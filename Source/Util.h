/* =========================================================================
    Util.h
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/05 19:22:48 R-Koubou
               1st Edition.

   ======================================================================== */

#pragma once

#include <windows.h>
#include "LocalFreeDBItem.h"

using namespace System;
using namespace System::Threading;

/**
 * @author R-Koubou
 */
public ref class Util
{
//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
private:

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Ctor.
     */
    Util( void ){}


public:

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * Sleep with given arg (millis)
     */
    static void sleep( unsigned int ms )
    {
        Thread::Sleep( ms );
    }

    //////////////////////////////////////////////////////////////////////////
    /**
     *
     */
    static void Util::printCleanupCommand();

    //////////////////////////////////////////////////////////////////////////
    /**
     *
     */
    static String^ readCommandLineFile( String^ file );

    //////////////////////////////////////////////////////////////////////////
    /**
     *
     */
    static String^ escapeShellCharacter( String^ msg );

    //////////////////////////////////////////////////////////////////////////
    /**
     *
     */
    static String^ escapeShellArgCharacter( String^ arg );

    //////////////////////////////////////////////////////////////////////////
    /**
     *
     */
    static String^ parseCdda2WavCommandParameter( String^ cddevId, String^ prefix, String^ outputDir, String^ artist, String^ album, String^ cmd );

    //////////////////////////////////////////////////////////////////////////
    /**
     *
     */
    static String^ parseEncoderCommandParameter( LocalFreeDBItem^ item, int trackNo, int trackNum, String^ trackName, String^ artworkFile, String^ cdda2wavDefaultPrefix, String^ prefix, String^ outputName, String^ outputDir, String^ cmd );

};
