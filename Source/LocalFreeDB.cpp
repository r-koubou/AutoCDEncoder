/* =========================================================================
    LocalFreeDB.cpp
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/06 23:30:39 R-Koubou
               1st Edition.

   ======================================================================== */

#include "LocalFreeDB.h"


//------------------------------------------------------------------------------
// File scope C Function
//------------------------------------------------------------------------------
static void searchAllFiles( String^ dir, ArrayList^ dest )
{
    array<String^>^ files = Directory::GetFiles( dir, "*" );

    //ArrayListに追加する
    dest->AddRange( files );

    //folderのサブフォルダを取得する
    array<String^>^ ds = Directory::GetDirectories( dir );

    //サブフォルダにあるファイルも調べる
    for each( String^ d in ds )
    {
        searchAllFiles( d, dest );
    }
}

//////////////////////////////////////////////////////////////////////////
/**
 * Ctor.
 */
LocalFreeDB::LocalFreeDB( String^ localCddbDir, String^ encoding, CDDA_TOC* pCddaToc )
{

   //------------------------------------------------------------------------------
    // Correct XMCD format files
    //------------------------------------------------------------------------------
    db = gcnew List<List<LocalFreeDBItem^>^>( 5120 );

    ArrayList^ allFiles = gcnew ArrayList( 1024 );
    searchAllFiles( localCddbDir, allFiles );

    for each( String^ var in allFiles )
    {
        List<LocalFreeDBItem^>^ items;

        if( var->EndsWith( ".txt" ) || var->EndsWith( ".TXT" ) )
        {
            continue;
        }

        items = LocalFreeDBItem::create( var, encoding, pCddaToc );
        if( items->Count > 0 )
        {
            db->Add( items );
        }
        else
        {
            //Util::ConsoleEcho( "[WARN] items is 0 : {0}", var );
        }
    }
    //System::Console::WriteLine( "[INFO] DB Registerd songs: {0}", getRegisterdSongCount() );
    UNREFERENCEIF( allFiles );
}

//////////////////////////////////////////////////////////////////////////
/**
 * Search a cddb from given diskid.
 */
BOOL LocalFreeDB::exist( String^ discId )
{
    discId = discId->ToUpper();

    // Too heavy..... hmmmmm
    for each( List<LocalFreeDBItem^>^ i in db )
    {
        for each( LocalFreeDBItem^ j in i )
        {
            if( j->discId == discId )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////
/**
 * Get a cddb from given diskid.
 * @return if not found, return nullptr.
 */
LocalFreeDBItem^ LocalFreeDB::get( String^ discId )
{
    discId = discId->ToUpper();

    // Too heavy..... hmmmmm
    for each( List<LocalFreeDBItem^>^ i in db )
    {
        for each( LocalFreeDBItem^ j in i )
        {
            if( j->discId == discId )
            {
                return j;
            }
        }
    }
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
/**
 * Get registered songs count.
 */
int LocalFreeDB::getRegisterdSongCount()
{
    int count = 0;
    // Too heavy..... hmmmmm
    for each( List<LocalFreeDBItem^>^ i in db )
    {
        for each( LocalFreeDBItem^ j in i )
        {
            count += j->trackTitle->Count;
        }
    }
    return count;
}
