/* =========================================================================
    Main.cpp
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/04 0:39:16 R-Koubou
               1st Edition.

   ======================================================================== */

#include "AutoCDEncoder.h"

using namespace System;
using namespace System::Windows::Forms;

//////////////////////////////////////////////////////////////////////////
/**
 * アプリケーション・エントリポイント
 */
#pragma managed
int main( array <String^> ^args )
{
    if( args->Length < 1 )
    {
        return 1;
    }

    Properties^ argv = gcnew Properties();
    argv->load( args[ 0 ], "utf-8" );

    String^ debugMode     = argv->getProperty( "DEBUG", "false" );

    String^ encodeThread  = argv->getProperty( "ENCODE_THREAD_NUM", "1" );
    String^ outputDir     = argv->getProperty( "OUTPUT_DIR" );

    String^ cdda2wavCmd   = argv->getProperty( "CDDA2WAV_CMD" );
    String^ cdda2wavDevId = argv->getProperty( "CDDA2WAV_DEVICE_ID" );
    String^ cdda2wavArg   = argv->getProperty( "CDDA2WAV_ARGS_FILE" );
    String^ cdda2wavPrefix= argv->getProperty( "CDDA2WAV_DEFAULT_PREFIX" );
    String^ cdromPath     = argv->getProperty( "CDROM_DRIVE" );

    String^ cddbPath      = argv->getProperty( "LOCALCDDA_DIR" );
    String^ encoding      = argv->getProperty( "XMCD_ENCODING" );
    String^ encoderCmd    = argv->getProperty( "ENCODER_CMD" );
    String^ encoderArg    = argv->getProperty( "ENCODER_ARGS_FILE" );
    String^ prefix        = argv->getProperty( "PREFIX" );
    String^ suffix        = argv->getProperty( "SUFFIX" );
    String^ artwork       = argv->getProperty( "ATRWORK_FILE" );

    if( CLI_ISNULL( outputDir )     ||
        CLI_ISNULL( cdda2wavCmd )   ||
        CLI_ISNULL( cdda2wavDevId ) ||
        CLI_ISNULL( cdda2wavArg )   ||
        CLI_ISNULL( cdda2wavPrefix )||
        CLI_ISNULL( cdromPath )     ||
        CLI_ISNULL( cddbPath )      ||
        CLI_ISNULL( encoding )      ||
        CLI_ISNULL( prefix )        ||
        CLI_ISNULL( suffix ) )
    {
        System::Console::WriteLine( "Parameter is Invalie." );
        return 1;
    }

#if 0
    cdda2wavArg = Application::StartupPath + "\\" + cdda2wavArg;
    encoderArg  = Application::StartupPath + "\\" + encoderArg;
#else
    cdda2wavArg =  cdda2wavArg;
    encoderArg = encoderArg;
#endif

    CDDA_TOC toc = { 0 };
    BOOL bResult;

    UINT32 id    = CDDA_GetDiscID( cdromPath[0], &toc, &bResult );
    if( bResult != TRUE )
    {
        System::Console::WriteLine( "[ERROR] cannot read toc : " + cdromPath );
        return 1;
    }

    String^ discId = String::Format( "{0:X8}", id );
    LocalFreeDB^ db  = gcnew LocalFreeDB( cddbPath, encoding, &toc );
    LocalFreeDBItem^ item = db->get( discId );

    if( item == nullptr )
    {
        System::Console::WriteLine( "[WARN] cddb not found : " + discId );
        return 1;
    }

    EncodeExecutor^ exec = gcnew EncodeExecutor( bool::Parse( debugMode ),
                                                 int::Parse( encodeThread ),
                                                 &toc,
                                                 item,
                                                 outputDir,
                                                 cdda2wavCmd,
                                                 cdda2wavDevId,
                                                 cdda2wavArg,
                                                 cdda2wavPrefix,
                                                 encoderCmd,
                                                 encoderArg,
                                                 prefix,
                                                 suffix,
                                                 artwork
    );

    exec->execute();

    return 0;
}
