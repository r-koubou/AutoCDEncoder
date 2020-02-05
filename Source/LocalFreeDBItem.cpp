/* =========================================================================
    LocalFreeDBItem.cpp
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/06 21:10:45 R-Koubou
               1st Edition.

   ======================================================================== */

#include "LocalFreeDBItem.h"


////////////////////////////////////////////////////////////////////////////////
/**
 * Factory Method.
 */
List<LocalFreeDBItem^>^ LocalFreeDBItem::create( String^ path, String^ encoding, CDDA_TOC* pCddaToc )
{
    List<LocalFreeDBItem^>^ list = nullptr;
    LocalFreeDBItem^ item        = nullptr;

    String^ discId               = nullptr;

    StreamReader ^reader         = nullptr;

    try
    {
        list = gcnew List<LocalFreeDBItem^>();
        item = gcnew LocalFreeDBItem();

        //------------------------------------------------------------------------------
        // TOC の値コピー
        //------------------------------------------------------------------------------
        *item->pCddaToc = *pCddaToc;

        reader = gcnew StreamReader( path, System::Text::Encoding::GetEncoding( encoding ) );

        String^ line            = String::Empty;
        Regex^ DELIMITER        = gcnew Regex( "\\s*=\\s*" );
        Regex^ DTITLE_DELIMITER = gcnew Regex( " / " );

        while( reader->Peek() >= 0 )
        {
            line = reader->ReadLine();
            line = line->Trim();

            if( line == String::Empty || line->StartsWith( "#" ) )
            {
                continue;
            }

            //------------------------------------------------------------------------------
            // DISCID
            //------------------------------------------------------------------------------
            if( line->StartsWith( "DISCID" ) )
            {
                if( discId != nullptr )
                {
                    // Maybe a new DISC info apperaed (Multiple DISC informations in this file)
                    item->discId = discId;
                    list->Add( item );

                    item   = gcnew LocalFreeDBItem();
                    discId = DELIMITER->Split( line )[ 1 ]->ToUpper();
                }
                else
                {
                    discId = DELIMITER->Split( line )[ 1 ]->ToUpper();
                }
            }
            //------------------------------------------------------------------------------
            // DTITLE
            //------------------------------------------------------------------------------
            else if( line->StartsWith( "DTITLE" ) )
            {
                String^ art;
                String^ alb;

                CLISTRING_ARRAY spl = DELIMITER->Split( line );
                if( spl->Length < 2 )
                {
                    art = "Unknown Artist";
                    alb = "Unknown Album";
                }
                else
                {
                    String^ value  = spl[ 1 ];
                    CLISTRING_ARRAY info = DTITLE_DELIMITER->Split( value );

                    art = info[ 0 ]->Trim();

                    // [Special processing]
                    // if " / " included in artist name. Try parse last index is album name.
                    if( info->Length > 2 )
                    {
                        // a "/" is FileSeparator as Unix. So guard from unknown issues.
                        art += " & ";
                        for( int i = 1; i < info->Length - 1; i++ )
                        {
                            art += info[ i ];
                            if( i + 1 < info->Length - 1 )
                            {
                                art += " & ";
                            }
                        }
                    }
                    alb = info[ info->Length - 1 ];

                    item->artist = art;
                    item->album  = alb;
                }
            }
            //------------------------------------------------------------------------------
            // DYEAR
            //------------------------------------------------------------------------------
            else if( line->StartsWith( "DYEAR" ) )
            {
                CLISTRING_ARRAY spl  = DELIMITER->Split( line );
                if( spl->Length > 1 )
                {
                    String^ y = spl[ 1 ]->Trim();
                    if( y->Length > 0 )
                    {
                        item->year = y;
                    }
                }
            }
            //------------------------------------------------------------------------------
            // DGENRE
            //------------------------------------------------------------------------------
            else if( line->StartsWith( "DGENRE" ) )
            {
                CLISTRING_ARRAY spl  = DELIMITER->Split( line );
                if( spl->Length > 1 )
                {
                    String^ g = spl[ 1 ]->Trim();
                    if( g->Length > 0 )
                    {
                        item->genre = g;
                    }
                }
            }
            //------------------------------------------------------------------------------
            // TTITLE
            //------------------------------------------------------------------------------
            else if( line->StartsWith( "TTITLE" ) )
            {
                CLISTRING_ARRAY spl = DELIMITER->Split( line );
                if( spl->Length > 1 )
                {
                    String^ name = spl[ 1 ]->Trim();

                    /*
                     * Quote from : THE FREEDB FILE FORMAT Specification
                     *
                     *      "The track number should be substituted for the "N", starting with 0."
                     *
                     * So, I adding track name "without check a track number". Because it written order by 0, 1, 2 .... N in the xmcd file.
                     */
                    if( name->Length > 0 )
                    {
                        item->trackTitle->Add( name );
                    }
                }
            }
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

    // at least more than 1 entries in CDDB information.
    if( discId != nullptr )
    {
        item->discId = discId;
        list->Add( item );
    }

    return list;

}

//////////////////////////////////////////////////////////////////////////
/**
 * dump
 */
void LocalFreeDBItem::dump()
{
    System::Console::WriteLine( "*********************************" );
    System::Console::WriteLine( "Artist : " + artist );
    System::Console::WriteLine( "Album  : " + album );
    System::Console::WriteLine( "Year   : " + year );
    System::Console::WriteLine( "Genre  : " + genre );
    System::Console::WriteLine( "---------- TRACK ---------------" );

    int i = 0;

    for each( String^ v in trackTitle )
    {
        System::Console::WriteLine( "Track {0:d2}: {1}", i, v );
        ++i;
    }

}

