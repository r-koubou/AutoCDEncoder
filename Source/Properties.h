/* =========================================================================
    Properties.h
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/07 15:32:49 R-Koubou
               1st Edition.

   ======================================================================== */

#pragma once

#include <windows.h>
#include "Macros.h"

#using <System.dll>

using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Text::RegularExpressions;

/**
 * Emulate java.util.Properties. 
 * @author R-Koubou
 */
public ref class Properties
{
//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------
private:

    Dictionary<String^,String^>^ propertyTable;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

public:

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Ctor.
     */
    Properties( void )
    {
        propertyTable = gcnew Dictionary<String^,String^>( 16 );
    }

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Dtor.
     */
    ~Properties( void ){}

    ////////////////////////////////////////////////////////////////////////////
    /**
     * Load from *.properties
     */
    void load( String^ file, String^ encoding );

    ////////////////////////////////////////////////////////////////////////////
    /**
     *
     */
    String^ getProperty( String^ key );

    ////////////////////////////////////////////////////////////////////////////
    /**
     *
     */
    String^ getProperty( String^ key, String^ defaultValue );

};
