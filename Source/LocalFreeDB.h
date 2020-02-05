/* =========================================================================
    LocalFreeDB.h
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/06 23:30:39 R-Koubou
               1st Edition.

   ======================================================================== */

#pragma once

#include <windows.h>
#include "LocalFreeDBItem.h"

using namespace System;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Text::RegularExpressions;

/**
 * @author R-Koubou
 */
public ref class LocalFreeDB
{
//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------

private:

    List<List<LocalFreeDBItem^>^>^db;

public:

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

public:

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Ctor.
     */
    LocalFreeDB( String^ localCddbDir, String^ encoding, CDDA_TOC* pCddaToc );

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Dtor.
     */
    virtual ~LocalFreeDB( void ){}

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Finalizer
     */
    !LocalFreeDB( void ){}

    //////////////////////////////////////////////////////////////////////////
    /**
     * Search a cddb from given diskid.
     */
    BOOL exist( String^ discId );

    //////////////////////////////////////////////////////////////////////////
    /**
     * Get a cddb from given diskid.
     * @return if not found, return nullptr.
     */
    LocalFreeDBItem^ get( String^ discId );

    //////////////////////////////////////////////////////////////////////////
    /**
     * Get registered songs count.
     */
    int getRegisterdSongCount();

};
