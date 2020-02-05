/* =========================================================================
    CDDA.cpp
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/06 3:22:59 R-Koubou
               1st Edition.

   ======================================================================== */

#include "CDDA.h"

//------------------------------------------------------------------------------
// C関数プロトタイプ宣言
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/**
 * Implementation of sum for freeDB Specification.
 */
static int cddb_sum( int n );


////////////////////////////////////////////////////////////////////////////////
/**
 * 指定ドライブ文字をCD-ROMとしてアクセスを試み、DISCIDを算出する。
 * @param [in]  drive ドライブ文字 例:  L"D"
 * @param [out] pDestToc CDから読み込んだTOC情報
 * @param [out] pResult 処理結果
 * @return DISCID
 */
#pragma managed
UINT32 CDDA_GetDiscID( WCHAR drive, CDDA_TOC* pDestToc, BOOL* pResult )
{
    HANDLE driveHandle = INVALID_HANDLE_VALUE;

    *pResult = FALSE;

    if( pDestToc == NULL )
    {
        return 0;
    }
    ::ZeroMemory( pDestToc, sizeof( CDDA_TOC ) );

    try
    {
        WCHAR drivePath[] = L"\\\\.\\L:";
        drivePath[ 4 ] = drive;
        // open a device
        driveHandle = ::CreateFile( drivePath,
                                    GENERIC_READ,
                                    FILE_SHARE_READ
                                    ,NULL
                                    ,OPEN_EXISTING
                                    ,FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN,
                                    (HANDLE)NULL
        );

        if( driveHandle == INVALID_HANDLE_VALUE )
        {
            return 0;
        }

        //------------------------------------------------------------------------------
        // Get TOC
        //------------------------------------------------------------------------------
        {
            BOOL  bResult;
            DWORD bytesResult;

            int sizeFtd = sizeof( CDROM_TOC_FULL_TOC_DATA );

            pin_ptr<BYTE> pBuffer;
            array<BYTE>^ buffer = nullptr;
            ALLOCATE_ARRAY( BYTE, buffer, sizeFtd, pBuffer );

            CDROM_READ_TOC_EX tocEx = { 0 };
            tocEx.Format = CDROM_READ_TOC_EX_FORMAT_FULL_TOC;
            tocEx.Msf    = 1;

            CDROM_TOC_FULL_TOC_DATA* pFtd = NULL;
            CDDA_TOC cddaToc;
            CDROM_SESSION session;

            //------------------------------------------------------------------------------
            // Get CDROM_TOC_FULL_TOC_DATA
            //------------------------------------------------------------------------------
            pFtd=(CDROM_TOC_FULL_TOC_DATA*)pBuffer;

            bResult = ::DeviceIoControl(
                            driveHandle,
                            IOCTL_CDROM_READ_TOC_EX,
                            &tocEx,
                            sizeof(CDROM_READ_TOC_EX),
                            pFtd,
                            sizeFtd,
                            &bytesResult,
                            NULL
            );

            if( bResult != TRUE )
            {
                return 0;
            }

            //------------------------------------------------------------------------------
            // Get CDROM_READ_TOC_EX
            //------------------------------------------------------------------------------
            sizeFtd = ( pFtd->Length[ 0 ] << 8 ) + pFtd->Length[ 1 ] + sizeof(UCHAR) * 2;
            if( sizeFtd % 4 != 0 ){ sizeFtd = ( sizeFtd / 4 + 1 ) * 4; }

            RESIZE_ARRAY2( buffer, sizeFtd, pBuffer );
            pFtd = (CDROM_TOC_FULL_TOC_DATA*)&pBuffer[ 0 ];

            bResult = ::DeviceIoControl(
                            driveHandle,
                            IOCTL_CDROM_READ_TOC_EX,
                            &tocEx,
                            sizeof(CDROM_READ_TOC_EX),
                            pFtd,
                            sizeFtd,
                            &bytesResult,
                            NULL
            );

            if( bResult != TRUE )
            {
                return 0;
            }

            //------------------------------------------------------------------------------
            // READ CDDA
            // Quote from http://hiroshi0945.seesaa.net/article/189083823.html
            //------------------------------------------------------------------------------

            //CDDAデータのあるセッション番号
            UINT sessionCDDA;

            //CDDA_TOC構造体メンバの初期化
            ::ZeroMemory( &cddaToc,sizeof(CDDA_TOC) );
            //CDROM_SESSION構造体メンバの初期化
            ::ZeroMemory( &session, sizeof(CDROM_SESSION) ) ;

            //セッション番号の初期化
            sessionCDDA             = 0;    //CDDAデータのあるセッションが見当たらない場合は０
            session.FirstSession    = 1;    //開始セッション番号は1
            session.LastSession     = 0;    //終了セッション番号を０にします。

            //トラック番号の初期化
            cddaToc.FirstTrack = 1;         //開始トラック番号は１
            cddaToc.LastTrack  = 0;         //終了トラック番号を０にします。

            //読み込んだCDROM_TOC_FULL_TOC_DATA構造体のサイズから、
            //Descriptors要素のCDROM_TOC_FULL_TOC_DATA_BLOCK構造体の個数を算出します。
            int nBlock = ( bytesResult - sizeof(CDROM_TOC_FULL_TOC_DATA) ) / sizeof(CDROM_TOC_FULL_TOC_DATA_BLOCK);
            UCHAR EndAddress[ 4 ] = { 0 };
            for( int i = 0; i < nBlock; i++ )
            {
                //「NCITS XXX T10/1363-D Revision - 02A」の「Table 350」を参照して下さい。
                CDROM_TOC_FULL_TOC_DATA_BLOCK* pDesc=&pFtd->Descriptors[i];
                int sessionNumber = pDesc->SessionNumber;
                switch( pDesc->Point )
                {
                    //------------------------------------------------------------------------------
                    //セッション開始トラック番号
                    //------------------------------------------------------------------------------
                    case 0xA0: 
                    {
                        session.sessionInfo[ sessionNumber - 1 ].SessionStart = pDesc->Msf[ 0 ];
                    }
                    break;
                    //------------------------------------------------------------------------------
                    //セッション終了トラック番号
                    //------------------------------------------------------------------------------
                    case 0xA1:
                    {
                        session.sessionInfo[ sessionNumber - 1 ].SessionStop = pDesc->Msf[ 0 ];
                    }
                    break;
                    //------------------------------------------------------------------------------
                    //セッションの終端位置の場合
                    //------------------------------------------------------------------------------
                    case 0xA2:
                    {
                        //セッションの終端位置のセクタ番号を格納します。
                        EndAddress[ 0 ] = 0;
                        EndAddress[ 1 ] = pDesc->Msf[ 0 ];
                        EndAddress[ 2 ] = pDesc->Msf[ 1 ];
                        EndAddress[ 3 ] = pDesc->Msf[ 2 ];
                        session.sessionInfo[ sessionNumber - 1 ].SessionEndFAD = MSF2FAD( EndAddress ) - 150;
                    }
                    break;

                    case 0xB0:
                    case 0xC0:
                        break;
                    //------------------------------------------------------------------------------
                    //Pointフィールドが1から99までで、現在の位置データの場合。
                    //------------------------------------------------------------------------------
                    default:
                    {
                        if( ( pDesc->Point >= 1 ) && ( pDesc->Point <= 0x63 ) && ( pDesc->Adr == ADR_ENCODES_CURRENT_POSITION ) )
                        {
                            //トラック番号の取得
                            UINT track = pDesc->Point;
                            //Q Sub-Channel属性を格納します。
                            cddaToc.TrackData[ track - 1 ].Addr = pDesc->Adr;
                            //トラックの属性を格納します。
                            cddaToc.TrackData[ track - 1 ].Control = pDesc->Control;
                            //開始セクタ番号を格納します。
                            cddaToc.TrackData[ track - 1 ].Address[ 0 ] = 0;
                            cddaToc.TrackData[ track - 1 ].Address[ 1 ] = pDesc->Msf[ 0 ];
                            cddaToc.TrackData[ track - 1 ].Address[ 2 ] = pDesc->Msf[ 1 ];
                            cddaToc.TrackData[ track - 1 ].Address[ 3 ] = pDesc->Msf[ 2 ];
                            cddaToc.TrackData[ track - 1 ].Session=sessionNumber;
                            //トラック番号が終了トラック番号より大きい場合、終了トラック番号に設定します。
                            if( cddaToc.LastTrack < track )
                            {
                                cddaToc.LastTrack=track;
                            }

                            //セッション番号に変更がある場合。
                            if( session.LastSession != sessionNumber )
                            {
                                session.LastSession = sessionNumber;
                                //セッション開始セクタ番号に、現在のトラック開始セクタ番号を代入します。
                                session.sessionInfo[ sessionNumber - 1 ].SessionStartFAD = MSF2FAD( cddaToc.TrackData[ track-1 ].Address );
                                //トラックがデータトラックでない場合は、セッション番号を保存します。
                                //「NCITS XXX T10/1363-D Revision - 02A」の「Table 337」を参照して下さい。
                                if( ( !sessionCDDA ) && ( ! (cddaToc.TrackData[ track - 1 ].Control & 4 ) ) )
                                {
                                    sessionCDDA = sessionNumber;
                                }
                            }
                        }
                    }
                    break;
                }
            } //~for( int i = 0; i < nBlock; i++ )

            //最終トラックの次のトラックに、最終セッション終了セクタ位置を書き込みます。
            cddaToc.TrackData[cddaToc.LastTrack].Address[ 0 ] = EndAddress[ 0 ];
            cddaToc.TrackData[cddaToc.LastTrack].Address[ 1 ] = EndAddress[ 1 ];
            cddaToc.TrackData[cddaToc.LastTrack].Address[ 2 ] = EndAddress[ 2 ];
            cddaToc.TrackData[cddaToc.LastTrack].Address[ 3 ] = EndAddress[ 3 ];

            // DISCID の計算
            {
                TRACKINFO* pTrack = &cddaToc.TrackData[ 0 ];
                int i;
                int sum      = 0;
                int secs     = AD2SEC( pTrack[ cddaToc.LastTrack ].Address ) - AD2SEC( pTrack[ 0 ].Address );
                int trackNum = cddaToc.LastTrack;

                for( i = 0; i < trackNum; i++ )
                {
                    sum = sum + cddb_sum( AD2SEC( pTrack[ i ].Address ) );
                }
                sum %= 255;

                *pResult  = TRUE;
                *pDestToc = cddaToc; // value copy

                return sum << 24 | secs << 8 | trackNum;
            }
        }
    }
    finally
    {
        RELEASEHANDLE( driveHandle );
    }

}

////////////////////////////////////////////////////////////////////////////////
/**
 * Implementation of sum for freeDB Specification.
 */
#pragma unmanaged
inline int cddb_sum( int n )
{
    int ret;

    /* For backward compatibility this algorithm must not change */

    ret = 0;

    while ( n > 0)
    {
        ret = ret + ( n % 10 );
        n = n / 10;
    }
    return ret;
}
