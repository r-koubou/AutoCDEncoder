/* =========================================================================
    Macros.h
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/04 22:55:27 R-Koubou
               1st Edition.

   ======================================================================== */

#pragma once

#include <windows.h>

using namespace System;

/** Msf時間からセクタ番号を算出。 */
#define MSF2FAD(msf) (((msf[1])*(CD_BLOCKS_PER_SECOND*60))+((msf[2])*CD_BLOCKS_PER_SECOND)+(msf[3]))

/** トラック情報(TRACKINFO.Address[])からフレーム単位に変換。 */
#define AD2FRAME( ad ) ( (ad[0]*3600*CD_BLOCKS_PER_SECOND) + (ad[1]*60*CD_BLOCKS_PER_SECOND) + (ad[2]*CD_BLOCKS_PER_SECOND) + ad[3] )

/** トラック情報(TRACKINFO.Address[])から秒単位に変換。 */
#define AD2SEC( ad ) ( (ad[0]*3600) + (ad[1]*60) + (ad[2]) )

// 指定のビットの値取得
#define BIT0(v) ( ( (v) >> 0 ) & 0x1 )
#define BIT1(v) ( ( (v) >> 1 ) & 0x1 )
#define BIT2(v) ( ( (v) >> 2 ) & 0x1 )
#define BIT3(v) ( ( (v) >> 3 ) & 0x1 )
#define BIT4(v) ( ( (v) >> 4 ) & 0x1 )
#define BIT5(v) ( ( (v) >> 5 ) & 0x1 )
#define BIT6(v) ( ( (v) >> 6 ) & 0x1 )
#define BIT7(v) ( ( (v) >> 7 ) & 0x1 )

// DELETE
#define DELETEIF(p)      if( p ){ delete( p ); p = NULL; }
#define UNREFERENCEIF(p) if( p != nullptr ){ delete( p ); p = nullptr; }

// System::array
#define CLEAR_ARRAY( arr )              Array::Clear( (arr), 0, (arr)->Length );
#define ALLOCATE_ARRAY( type, arr, size, ptr ) UNREFERENCEIF(arr); arr = gcnew array<type>(size); CLEAR_ARRAY(arr); ptr = &arr[ 0 ];
#define RESIZE_ARRAY1( arr, size )      Array::Resize( (arr), size );
#define RESIZE_ARRAY2( arr, size, ptr ) Array::Resize( (arr), size ); ptr = &arr[ 0 ];

// CLI
#define CLISTRING_ARRAY array<String^>^
#define REF_CLOSEIF( p ) if( (p) != nullptr ){ (p)->Close(); UNREFERENCEIF( (p) ); }
#define CLI_ISNULL( p ) ( (p) == nullptr )

// HANDLE
#define RELEASEHANDLE( p ) { if( (p) != INVALID_HANDLE_VALUE ){ ::CloseHandle( (p) ); } }


// 簡易ロギング
#define DEBUGLOG( ... ) { System::Console::WriteLine( __VA_ARGS__ ); }