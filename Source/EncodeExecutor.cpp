/* =========================================================================
    EncodeExecutor.cpp
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/07 15:12:28 R-Koubou
               1st Edition.

   ======================================================================== */

#include "EncodeExecutor.h"

static const int THREADING = 4;

////////////////////////////////////////////////////////////////////////////
/**
 * Execute.
 */
void EncodeExecutor::execute( void )
{
if( !MKDIR_ONLY ){
//-------------------------------------------------------------------------------
// RELEASE Code
//-------------------------------------------------------------------------------
    String^ artist = item->artist;
    String^ album  = item->album;

    int trackNum   = item->trackTitle->Count;

    String^ sep    = "\\";

    outputDir      = outputDir + sep + Util::escapeShellCharacter( artist ) + sep + Util::escapeShellCharacter( album );

    List<String^>^ list = item->trackTitle;

    if( Directory::Exists( outputDir ) )
    {
        int i = 0;
        for each ( String^ var in Directory::EnumerateFiles( outputDir ) )
        {
            i++;
        }
        if( i > 0 )
        {
            System::Console::WriteLine( "[WARN] directory and file(s) are already exist. Do nothing. : {0}", outputDir );
            return;
        }
    }

    // mkdir
    Directory::CreateDirectory( outputDir );

    //------------------------------------------------------------------------------
    // Exec cdda2wav
    //------------------------------------------------------------------------------
    {
        String^ cdda2wavCommandArgv    = Util::readCommandLineFile( cdda2wavCommandArgvFile );
        Process^ p = gcnew Process();
        p->StartInfo->FileName         = cdda2wavCommand;
        p->StartInfo->Arguments        = Util::parseCdda2WavCommandParameter( cdda2wavDevId, prefix, "\"" + outputDir + "\"", artist, album, cdda2wavCommandArgv );
        p->StartInfo->WorkingDirectory = outputDir;
        p->StartInfo->CreateNoWindow   = true;
        p->Start();
        while( ! p->HasExited )
        {
            Util::sleep( 50 );
        }
    }
    //------------------------------------------------------------------------------
    // Encode
    //------------------------------------------------------------------------------
    {
        if( THREADING > 1 )
        {
            int freeQueueIndex = -1;

            array<EncodeQueue^>^ queue = gcnew array<EncodeQueue^>( THREADING );

            for( int i = 0; i < THREADING; i++ )
            {
                queue[ i ] = nullptr;
            }

            String^ orgArgv = Util::readCommandLineFile( encoderCommandArgvFile );

            for( int i = 0; i < trackNum; )
            {
                // Quote from http://homepage2.nifty.com/yota/dkoza_for_cdr/007.html
                // 0 or 2 2channel audio w/o pre-emphasis
                // 1 or 3 2channel audio w pre-emphasis
                // 4 or 6 DataTrack(interrupted)
                // 5 or 7 DataTrack(intcremental)
                if( toc->TrackData[ i ].Control >= 4 )
                {
                    // DataTrack
                    i++;
                    continue;
                }

                int freeQueueIndex = -1;
                int j = 0;

                for each( EncodeQueue^ q in queue )
                {
                    if( q == nullptr )
                    {
                        freeQueueIndex = j;
                        break;
                    }
                    j++;
                }

                if( freeQueueIndex >= 0 )
                {
                    int trackNo = i + 1;

                    String^ outputName = String::Format( "{0}_{1:d2}.{2}", prefix, trackNo, suffix );
                    String^ t = list[ i ];
                    String^ artwork = Directory::GetCurrentDirectory() + "\\" + artworkFile;
                    String^ argv    = Util::parseEncoderCommandParameter( item, trackNo, trackNum, t, artwork, cdda2wavDefaultPrefix, prefix, outputName, outputDir, orgArgv );

                    EncodeQueue^ q = gcnew EncodeQueue( toc, item, outputDir, encoderCommand, argv, prefix, suffix, artwork );
                    WaitCallback^ callback = gcnew WaitCallback( EncodeExecutor::executeEncodeFromThreadPool );

                    queue[ freeQueueIndex ] = q;

                    bool result = ThreadPool::QueueUserWorkItem( callback, q );
                    if( !result )
                    {
                        System::Console::WriteLine( "[ERROR] cannot launch encoder : {0} {1}", encoderCommand, argv );
                    }
                    i++;
                }
                else
                {
                    monitorThreadPool( queue );
                }
            }

            while( monitorThreadPool( queue ) != THREADING )
            {
                Util::sleep( 20 );
            }

            Util::sleep( 1000 );
        }
        else
        {
            int freeQueueIndex = -1;

            String^ orgArgv = Util::readCommandLineFile( encoderCommandArgvFile );

            for( int i = 0; i < trackNum; )
            {
                if( toc->TrackData[ i ].Control != 0 )
                {
                    // NOT Audio data.
                    continue;
                }

                int trackNo = i + 1;
                String^ outputName = String::Format( "{0}_{1:d2}.{2}", prefix, trackNo, suffix );
                String^ t = list[ i ];
                String^ artwork = Directory::GetCurrentDirectory() + "\\" + artworkFile;
                String^ argv    = Util::parseEncoderCommandParameter( item, trackNo, trackNum, t, artwork, cdda2wavDefaultPrefix, prefix, outputName, outputDir, orgArgv );

                EncodeQueue^ q = gcnew EncodeQueue( toc, item, outputDir, encoderCommand, argv, prefix, suffix, artwork );
                q->execute();
                i++;
            }
        }
        //------------------------------------------------------------------------------
        // Cleanup
        //------------------------------------------------------------------------------
        {
            array<String^>^ files = Directory::GetFiles( outputDir, "*" );
            for each( String^ v in files )
            {
                if( v->EndsWith( ".wav" )     ||
                    v->EndsWith( ".cddb" )    ||
                    v->EndsWith( ".cdindex" ) ||
                    v->EndsWith( ".cdtext" ) ||
                    v->EndsWith( ".inf" ) )
                {
                    FileInfo^ fileInfo = gcnew FileInfo( v );
                    fileInfo->Delete();
                }
            }
        }
    }
} else {
//-------------------------------------------------------------------------------
// DEBUG Code
//-------------------------------------------------------------------------------
    {
        String^ artist = item->artist;
        String^ album  = item->album;

        int trackNum   = item->trackTitle->Count;

        String^ sep    = "\\";

        outputDir      = outputDir + sep + Util::escapeShellCharacter( artist ) + sep + Util::escapeShellCharacter( album );
        System::Console::WriteLine( "mkdir: " + outputDir );

        List<String^>^ list = item->trackTitle;

        if( Directory::Exists( outputDir ) )
        {
            int i = 0;
            for each ( String^ var in Directory::EnumerateFiles( outputDir ) )
            {
                i++;
            }
            if( i > 0 )
            {
                System::Console::WriteLine( "[WARN] directory and file(s) are already exist. Do nothing. : {0}", outputDir );
                return;
            }
        }

        // mkdir only
        Directory::CreateDirectory( outputDir );
    }

} //~if( !MKDIR_ONLY ){
}

////////////////////////////////////////////////////////////////////////////
/**
 * Monitor my encoding threadpook.
 */
int EncodeExecutor::monitorThreadPool( array<EncodeQueue^>^ queue )
{
    int i = 0;
    int freeSpace = 0;
    for each( EncodeQueue^ q in queue )
    {
        if( q != nullptr )
        {
            if( q->finished )
            {
                queue[ i ] = nullptr;
                freeSpace++;
            }
        }
        else
        {
            freeSpace++;
        }
        i++;
    }
    return freeSpace;
}
