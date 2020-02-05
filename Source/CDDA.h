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
    //CD-DA�̃Z�N�^����̃o�C�g��
    RAW_SECTOR_SIZE         = 2352,
    //1�b�Ԃ�CD-DA���ǂݏo���Z�N�^��
    CD_BLOCKS_PER_SECOND    = 75
};

// Quote from http://hiroshi0945.seesaa.net/article/189083823.html
//TRACKINFO�\����
typedef struct _TRACKINFO
{
    BYTE    Control;    //�g���b�N�̑����i�ڂ����́uNCITS XXX T10/1363-D Revision - 02A
                        //�iTable 337 - Q Sub-channel control field�j�v���Q�Ƃ��ĉ������B�j
                        // -> ftp://ftp.avc-pioneer.com/Mtfuji_5/Output/mmc3r04_fuji.pdf

                        // Quote from http://homepage2.nifty.com/yota/dkoza_for_cdr/007.html
                        // 0 or 2 2channel audio w/o pre-emphasis
                        // 1 or 3 2channel audio w pre-emphasis
                        // 4 or 6 DataTrack(interrupted)
                        // 5 or 7 DataTrack(intcremental)

    BYTE    Addr;       //Q Sub-Channel�����i�ڂ����́uNCITS XXX T10/1363-D Revision - 02A
                        //�iTable 336 - ADR Q Sub-channel field�j�v</a>���Q�Ƃ��ĉ������B�j
                        // -> ftp://ftp.avc-pioneer.com/Mtfuji_5/Output/mmc3r04_fuji.pdf


                        // Quote from http://homepage2.nifty.com/yota/dkoza_for_cdr/007.html
                        // 0 SubQ-not Support
                        // 1 Sub-Q Support 
                        // 2 media catalogue number
                        // 3 ISRC


    BYTE    Session;    //��������Z�b�V�����ԍ�
    BYTE    Address[4]; //�J�n�Z�N�^�ԍ�
                        // [0] Hour
                        // [1] Minute
                        // [2] Second
                        // [3] Frame
} TRACKINFO;

// Quote from http://hiroshi0945.seesaa.net/article/189083823.html
//CDDA_TOC�\����
typedef struct _CDDA_TOC
{
    BYTE    FirstTrack; //�J�n�g���b�N
    BYTE    LastTrack;  //�I���g���b�N
    //�g���b�N���̏��
    TRACKINFO TrackData[MAXIMUM_NUMBER_TRACKS];

} CDDA_TOC;

// Quote from http://hiroshi0945.seesaa.net/article/189083823.html
//SESSIONINFO�\����
typedef struct _SESSIONINFO
{
    BYTE    SessionStart;    //�Z�b�V�����̊J�n�g���b�N�ԍ�
    BYTE    SessionStop;     //�Z�b�V�����̏I���g���b�N�ԍ�
    DWORD   SessionStartFAD; //�Z�b�V�����̊J�n�Z�N�^�ԍ� (=�g���b�N1�̃X�^�[�g�I�t�Z�b�g)
    DWORD   SessionEndFAD;   //�Z�b�V�����̏I���Z�N�^�ԍ�
} SESSIONINFO;

// Quote from http://hiroshi0945.seesaa.net/article/189083823.html
//CDROM_SESSION�\����
typedef struct _CDROM_SESSION
{
    BYTE    FirstSession;   //�ŏ��̃Z�b�V�����ԍ�
    BYTE    LastSession;    //�Ō�̃Z�b�V�����ԍ�
    //�Z�b�V�������̏��
    SESSIONINFO sessionInfo[MAXIMUM_NUMBER_TRACKS];
} CDROM_SESSION;


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/**
 * �w��h���C�u������CD-ROM�Ƃ��ăA�N�Z�X�����݁ADISCID���Z�o����B
 * @param [in]  drive �h���C�u���� ��:  L"D"
 * @param [out] pDestToc CD����ǂݍ���TOC���
 * @param [out] pResult ��������
 * @return DISCID
 */
UINT32 CDDA_GetDiscID( WCHAR drive, CDDA_TOC* pDestToc, BOOL* pResult );
