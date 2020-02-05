/* =========================================================================
    Util.cpp
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/05 19:22:48 R-Koubou
               1st Edition.

   ======================================================================== */

#include "Util.h"

//////////////////////////////////////////////////////////////////////////
/**
 *
 */
String^ Util::readCommandLineFile( String^ file )
{
    String^ text = String::Empty;
    Stream^ stream = nullptr;
    StreamReader^ reader = nullptr;
    try
    {
        reader = gcnew StreamReader( file, System::Text::Encoding::GetEncoding( "utf-8" ) );
        while( reader->Peek() >= 0 )
        {
            text += reader->ReadLine();
        }
    }
    finally
    {
        REF_CLOSEIF( reader );
        REF_CLOSEIF( stream );
    }

    return text;
}

//////////////////////////////////////////////////////////////////////////
/**
 *
 */
void Util::printCleanupCommand()
{
    // Windows
    System::Console::WriteLine( "del /q .\\*.cddb>nul" );
    System::Console::WriteLine( "del /q .\\*.cdindex>nul" );
    System::Console::WriteLine( "del /q .\\*.cdtext>nul" );
    System::Console::WriteLine( "del /q .\\*.inf>nul" );
    System::Console::WriteLine( "del /q .\\*.wav>nul" );
}

//////////////////////////////////////////////////////////////////////////
/**
 *
 */
String^ Util::escapeShellCharacter( String^ msg )
{
    msg = msg->Replace( "?", "H" );
    msg = msg->Replace( ":", "F" );
    msg = msg->Replace( "|", "b" );
    msg = msg->Replace( ">", "„" );
    msg = msg->Replace( "<", "ƒ" );
    msg = msg->Replace( "\"", "h" );
    msg = msg->Replace( "/", "^" );
    msg = msg->Replace( "*", "–" );
    return msg;
}

//////////////////////////////////////////////////////////////////////////
/**
 *
 */
String^ Util::escapeShellArgCharacter( String^ arg )
{
    String^ envCheck;
    arg     = arg->Trim();
    envCheck = arg;

    envCheck = envCheck->Replace( "\"", "" );
    if( envCheck->StartsWith( "%" ) && envCheck->EndsWith( "%" ) )
    {
        System::Console::WriteLine( "[WARN] arg is environment format %foo%" );
    }

    if( arg->StartsWith( "\"" ) && arg->EndsWith( "\"" ) )
    {
        arg = arg->Replace( "\"", "\\\"" );
    }
    else
    {
        arg = arg->Replace( "&", "•" );
        arg = arg->Replace( "|", "b" );
        arg = arg->Replace( "^", "O" );
        arg = arg->Replace( ">", "„" );
        arg = arg->Replace( "<", "ƒ" );
        arg = arg->Replace( "%", "“" );
    }
    return arg;
}

//////////////////////////////////////////////////////////////////////////
/**
 *
 */
String^ Util::parseCdda2WavCommandParameter( String^ cddevId, String^ prefix, String^ outputDir, String^ artist, String^ album, String^ cmd )
{
    cmd = cmd->Replace( "%did", cddevId );
    cmd = cmd->Replace( "%1",   prefix );
    cmd = cmd->Replace( "%a",   artist );
    cmd = cmd->Replace( "%b",   album );
    cmd = cmd->Replace( "%d",   outputDir );
    return cmd;
}

//////////////////////////////////////////////////////////////////////////
/**
 *
 */
String^ Util::parseEncoderCommandParameter( LocalFreeDBItem^ item, int trackNo, int trackNum, String^ trackName, String^ artworkFile, String^ cdda2wavDefaultPrefix, String^ prefix, String^ outputName, String^ outputDir, String^ cmd )
{
    String^ artist = escapeShellArgCharacter( item->artist );
    String^ album  = escapeShellArgCharacter( item->album  );
    String^ year   = escapeShellArgCharacter( item->year   );
    String^ genre  = escapeShellArgCharacter( item->genre  );
    trackName      = escapeShellArgCharacter( trackName );

    String^ src    = String::Format( "{0}_{1:d2}.wav" , cdda2wavDefaultPrefix, trackNo );
    if( trackNum == 1 )
    {
        // if 1 track only disc, cdda2wav don't append prefix '_xx'
        src = String::Format( "{0}.wav" , prefix );
    }

    cmd = cmd->Replace( "%1", src );
    cmd = cmd->Replace( "%2", outputName );
    cmd = cmd->Replace( "%d", outputDir );

    cmd = cmd->Replace( "%a", artist );
    cmd = cmd->Replace( "%b", album );
    cmd = cmd->Replace( "%tn", String::Format( "{0}", trackNo ) );
    cmd = cmd->Replace( "%tt", String::Format( "{0}", trackNum ) );
    cmd = cmd->Replace( "%t", trackName );
    cmd = cmd->Replace( "%y", year );
    cmd = cmd->Replace( "%g", genre );

    if( artworkFile != nullptr )
    {
        cmd = cmd->Replace( "%p", artworkFile );
    }

    return cmd;
}
