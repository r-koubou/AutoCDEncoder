/* =========================================================================
    CDDA.cpp
   -------------------------------------------------------------------------

   [History]
   Ver 1.00:   2013/04/06 3:22:59 R-Koubou
               1st Edition.

   ======================================================================== */

#include "CDDA.h"

//------------------------------------------------------------------------------
// C�֐��v���g�^�C�v�錾
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/**
 * Implementation of sum for freeDB Specification.
 */
static int cddb_sum( int n );


////////////////////////////////////////////////////////////////////////////////
/**
 * �w��h���C�u������CD-ROM�Ƃ��ăA�N�Z�X�����݁ADISCID���Z�o����B
 * @param [in]  drive �h���C�u���� ��:  L"D"
 * @param [out] pDestToc CD����ǂݍ���TOC���
 * @param [out] pResult ��������
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

            //CDDA�f�[�^�̂���Z�b�V�����ԍ�
            UINT sessionCDDA;

            //CDDA_TOC�\���̃����o�̏�����
            ::ZeroMemory( &cddaToc,sizeof(CDDA_TOC) );
            //CDROM_SESSION�\���̃����o�̏�����
            ::ZeroMemory( &session, sizeof(CDROM_SESSION) ) ;

            //�Z�b�V�����ԍ��̏�����
            sessionCDDA             = 0;    //CDDA�f�[�^�̂���Z�b�V��������������Ȃ��ꍇ�͂O
            session.FirstSession    = 1;    //�J�n�Z�b�V�����ԍ���1
            session.LastSession     = 0;    //�I���Z�b�V�����ԍ����O�ɂ��܂��B

            //�g���b�N�ԍ��̏�����
            cddaToc.FirstTrack = 1;         //�J�n�g���b�N�ԍ��͂P
            cddaToc.LastTrack  = 0;         //�I���g���b�N�ԍ����O�ɂ��܂��B

            //�ǂݍ���CDROM_TOC_FULL_TOC_DATA�\���̂̃T�C�Y����A
            //Descriptors�v�f��CDROM_TOC_FULL_TOC_DATA_BLOCK�\���̂̌����Z�o���܂��B
            int nBlock = ( bytesResult - sizeof(CDROM_TOC_FULL_TOC_DATA) ) / sizeof(CDROM_TOC_FULL_TOC_DATA_BLOCK);
            UCHAR EndAddress[ 4 ] = { 0 };
            for( int i = 0; i < nBlock; i++ )
            {
                //�uNCITS XXX T10/1363-D Revision - 02A�v�́uTable 350�v���Q�Ƃ��ĉ������B
                CDROM_TOC_FULL_TOC_DATA_BLOCK* pDesc=&pFtd->Descriptors[i];
                int sessionNumber = pDesc->SessionNumber;
                switch( pDesc->Point )
                {
                    //------------------------------------------------------------------------------
                    //�Z�b�V�����J�n�g���b�N�ԍ�
                    //------------------------------------------------------------------------------
                    case 0xA0: 
                    {
                        session.sessionInfo[ sessionNumber - 1 ].SessionStart = pDesc->Msf[ 0 ];
                    }
                    break;
                    //------------------------------------------------------------------------------
                    //�Z�b�V�����I���g���b�N�ԍ�
                    //------------------------------------------------------------------------------
                    case 0xA1:
                    {
                        session.sessionInfo[ sessionNumber - 1 ].SessionStop = pDesc->Msf[ 0 ];
                    }
                    break;
                    //------------------------------------------------------------------------------
                    //�Z�b�V�����̏I�[�ʒu�̏ꍇ
                    //------------------------------------------------------------------------------
                    case 0xA2:
                    {
                        //�Z�b�V�����̏I�[�ʒu�̃Z�N�^�ԍ����i�[���܂��B
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
                    //Point�t�B�[���h��1����99�܂łŁA���݂̈ʒu�f�[�^�̏ꍇ�B
                    //------------------------------------------------------------------------------
                    default:
                    {
                        if( ( pDesc->Point >= 1 ) && ( pDesc->Point <= 0x63 ) && ( pDesc->Adr == ADR_ENCODES_CURRENT_POSITION ) )
                        {
                            //�g���b�N�ԍ��̎擾
                            UINT track = pDesc->Point;
                            //Q Sub-Channel�������i�[���܂��B
                            cddaToc.TrackData[ track - 1 ].Addr = pDesc->Adr;
                            //�g���b�N�̑������i�[���܂��B
                            cddaToc.TrackData[ track - 1 ].Control = pDesc->Control;
                            //�J�n�Z�N�^�ԍ����i�[���܂��B
                            cddaToc.TrackData[ track - 1 ].Address[ 0 ] = 0;
                            cddaToc.TrackData[ track - 1 ].Address[ 1 ] = pDesc->Msf[ 0 ];
                            cddaToc.TrackData[ track - 1 ].Address[ 2 ] = pDesc->Msf[ 1 ];
                            cddaToc.TrackData[ track - 1 ].Address[ 3 ] = pDesc->Msf[ 2 ];
                            cddaToc.TrackData[ track - 1 ].Session=sessionNumber;
                            //�g���b�N�ԍ����I���g���b�N�ԍ����傫���ꍇ�A�I���g���b�N�ԍ��ɐݒ肵�܂��B
                            if( cddaToc.LastTrack < track )
                            {
                                cddaToc.LastTrack=track;
                            }

                            //�Z�b�V�����ԍ��ɕύX������ꍇ�B
                            if( session.LastSession != sessionNumber )
                            {
                                session.LastSession = sessionNumber;
                                //�Z�b�V�����J�n�Z�N�^�ԍ��ɁA���݂̃g���b�N�J�n�Z�N�^�ԍ��������܂��B
                                session.sessionInfo[ sessionNumber - 1 ].SessionStartFAD = MSF2FAD( cddaToc.TrackData[ track-1 ].Address );
                                //�g���b�N���f�[�^�g���b�N�łȂ��ꍇ�́A�Z�b�V�����ԍ���ۑ����܂��B
                                //�uNCITS XXX T10/1363-D Revision - 02A�v�́uTable 337�v���Q�Ƃ��ĉ������B
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

            //�ŏI�g���b�N�̎��̃g���b�N�ɁA�ŏI�Z�b�V�����I���Z�N�^�ʒu���������݂܂��B
            cddaToc.TrackData[cddaToc.LastTrack].Address[ 0 ] = EndAddress[ 0 ];
            cddaToc.TrackData[cddaToc.LastTrack].Address[ 1 ] = EndAddress[ 1 ];
            cddaToc.TrackData[cddaToc.LastTrack].Address[ 2 ] = EndAddress[ 2 ];
            cddaToc.TrackData[cddaToc.LastTrack].Address[ 3 ] = EndAddress[ 3 ];

            // DISCID �̌v�Z
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
