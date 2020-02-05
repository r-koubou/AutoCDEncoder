/* =========================================================================
    CDDA.h
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/04 22:11:59 R-Koubou
               1st Edition.

   ======================================================================== */

#pragma once

#include <windows.h>
#include <devioctl.h>
#include <ntddcdrm.h>

#include "Macros.h"

//------------------------------------------------------------------------------
// Variables
//------------------------------------------------------------------------------

enum
{
    //CD-DAのセクタ当りのバイト数
    RAW_SECTOR_SIZE         = 2352,
    //1秒間にCD-DAが読み出すセクタ数
    CD_BLOCKS_PER_SECOND    = 75
};

// Quote from http://hiroshi0945.seesaa.net/article/189083823.html
//TRACKINFO構造体
typedef struct _TRACKINFO
{
    BYTE    Control;    //トラックの属性（詳しくは「NCITS XXX T10/1363-D Revision - 02A
                        //（Table 337 - Q Sub-channel control field）」を参照して下さい。）
                        // -> ftp://ftp.avc-pioneer.com/Mtfuji_5/Output/mmc3r04_fuji.pdf

                        // Quote from http://homepage2.nifty.com/yota/dkoza_for_cdr/007.html
                        // 0 or 2 2channel audio w/o pre-emphasis
                        // 1 or 3 2channel audio w pre-emphasis
                        // 4 or 6 DataTrack(interrupted)
                        // 5 or 7 DataTrack(intcremental)

    BYTE    Addr;       //Q Sub-Channel属性（詳しくは「NCITS XXX T10/1363-D Revision - 02A
                        //（Table 336 - ADR Q Sub-channel field）」</a>を参照して下さい。）
                        // -> ftp://ftp.avc-pioneer.com/Mtfuji_5/Output/mmc3r04_fuji.pdf


                        // Quote from http://homepage2.nifty.com/yota/dkoza_for_cdr/007.html
                        // 0 SubQ-not Support
                        // 1 Sub-Q Support 
                        // 2 media catalogue number
                        // 3 ISRC


    BYTE    Session;    //所属するセッション番号
    BYTE    Address[4]; //開始セクタ番号
                        // [0] Hour
                        // [1] Minute
                        // [2] Second
                        // [3] Frame
} TRACKINFO;

// Quote from http://hiroshi0945.seesaa.net/article/189083823.html
//CDDA_TOC構造体
typedef struct _CDDA_TOC
{
    BYTE    FirstTrack; //開始トラック
    BYTE    LastTrack;  //終了トラック
    //トラック毎の情報
    TRACKINFO TrackData[MAXIMUM_NUMBER_TRACKS];

} CDDA_TOC;

// Quote from http://hiroshi0945.seesaa.net/article/189083823.html
//SESSIONINFO構造体
typedef struct _SESSIONINFO
{
    BYTE    SessionStart;    //セッションの開始トラック番号
    BYTE    SessionStop;     //セッションの終了トラック番号
    DWORD   SessionStartFAD; //セッションの開始セクタ番号 (=トラック1のスタートオフセット)
    DWORD   SessionEndFAD;   //セッションの終了セクタ番号
} SESSIONINFO;

// Quote from http://hiroshi0945.seesaa.net/article/189083823.html
//CDROM_SESSION構造体
typedef struct _CDROM_SESSION
{
    BYTE    FirstSession;   //最初のセッション番号
    BYTE    LastSession;    //最後のセッション番号
    //セッション毎の情報
    SESSIONINFO sessionInfo[MAXIMUM_NUMBER_TRACKS];
} CDROM_SESSION;


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/**
 * 指定ドライブ文字をCD-ROMとしてアクセスを試み、DISCIDを算出する。
 * @param [in]  drive ドライブ文字 例:  L"D"
 * @param [out] pDestToc CDから読み込んだTOC情報
 * @param [out] pResult 処理結果
 * @return DISCID
 */
UINT32 CDDA_GetDiscID( WCHAR drive, CDDA_TOC* pDestToc, BOOL* pResult );
