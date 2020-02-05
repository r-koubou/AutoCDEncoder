/* =========================================================================
    Properties.cpp
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/07 15:32:49 R-Koubou
               1st Edition.

   ======================================================================== */

#include "Properties.h"

////////////////////////////////////////////////////////////////////////////
/**
 * Load from *.properties
 */
void Properties::load( String^ file, String^ encoding )
{
    StreamReader ^reader         = nullptr;

    try
    {
        reader = gcnew StreamReader( file, System::Text::Encoding::GetEncoding( encoding ) );

        String^ line            = String::Empty;
        Regex^ DELIMITER        = gcnew Regex( "\\s*=\\s*" );

        while( reader->Peek() >= 0 )
        {
            line = reader->ReadLine();
            line = line->Trim();

            if( line == String::Empty || line->StartsWith( "#" ) )
            {
                continue;
            }

            array<String^>^ data = DELIMITER->Split( line );
            propertyTable->Add( data[ 0 ], data[ 1 ] );
        }
    }
    catch( TypeLoadException^ e )
    {
        System::Console::WriteLine( e->StackTrace );
    }
    finally
    {
        UNREFERENCEIF( reader );
    }
}

////////////////////////////////////////////////////////////////////////////
/**
 *
 */
String^ Properties::getProperty( String^ key )
{
    return getProperty( key, nullptr );
}

////////////////////////////////////////////////////////////////////////////
/**
 *
 */
String^ Properties::getProperty( String^ key, String^ defaultValue )
{
    if( propertyTable->ContainsKey( key ) )
    {
        return propertyTable[ key ];
    }

    return defaultValue;

}