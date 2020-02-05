/* =========================================================================
    LocalFreeDBItem.h
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/06 21:10:45 R-Koubou
               1st Edition.

   ======================================================================== */

#pragma once

#include <windows.h>
#include "CDDA.h"

#using <System.dll>

using namespace System;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Text::RegularExpressions;

/**
 * @author R-Koubou
 */
public ref class LocalFreeDBItem
{
//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------
private:

    CDDA_TOC* pCddaToc;

public:

    List<String^>^trackTitle;

    String^ discId;
    String^ artist;
    String^ album;
    String^ year;
    String^ genre;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

private:

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Ctor.
     */
    LocalFreeDBItem( void )
    {
        trackTitle  = gcnew List<String^>( 32 );
        discId      = nullptr;
        artist      = "Unknown";
        album       = "Unknown";
        year        = "Unknown";
        genre       = "Unknown";
        pCddaToc    = new CDDA_TOC();
        ::ZeroMemory( pCddaToc, sizeof( CDDA_TOC ) );
    }
public:

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Dtor.
     */
    virtual ~LocalFreeDBItem( void )
    {
    }

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Finalizer.
     */
    !LocalFreeDBItem( void )
    {
        DELETEIF( pCddaToc );
    }

    ////////////////////////////////////////////////////////////////////////////////
    /**
     * Factory Method.
     */
    static List<LocalFreeDBItem^>^ create( String^ path, String^ encoding, CDDA_TOC* pCddaToc );

    //////////////////////////////////////////////////////////////////////////
    /**
     * dump
     */
    void dump();

};
