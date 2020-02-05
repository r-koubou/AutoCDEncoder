@echo off

setlocal

rem ------------------------------------------------------------------------------------------------------------
rem �w�肵���f�B���N�g���̃T�u�f�B���N�g���� *.cue �� image.jpg ���g�p���AAlcohol 52%�Ń}�E���g�A�G���R�[�h����
rem ------------------------------------------------------------------------------------------------------------

rem %1 profile
rem %2 cddrive
rem %3 cd-image root

if %1 == "" goto usage
if %2 == "" goto usage
if %3 == "" goto usage

set ENCODE_PROFILE=%~dp0\%1

set VIRTUAL_CD_DRIVE=%2
set MOUNT_CMD=AxCmd.exe %VIRTUAL_CD_DRIVE%:\
set UNMOUNT_CMD=AxCmd.exe %VIRTUAL_CD_DRIVE%:\ /U

set CDIMAGE_ROOT=%3


rem *.cue ���Ȃ��ꍇ�i�I�[�f�B�I�t�@�C���������������ꍇ�̊g���q�j
set LEGACY_FILE_SUFFIX1=mp3
set LEGACY_FILE_SUFFIX2=m4a

set ARGV=%ENCODE_PROFILE%
set ENCODER=%~dp0\AutoCDEncoder.exe
for /R %CDIMAGE_ROOT% %%i in (.) do (
    pushd %%i
    
        for %%j in (*.cue) do (
        
            @echo --------------------------------------------------------------------------------------
            @echo %%i\%%j
            @echo --------------------------------------------------------------------------------------
            if exist .ignore (
                echo [!] Ignore ripping and encode '.ignore found' : 
            ) else (

                rem ���z�h���C�u�Ƀ}�E���g
                %MOUNT_CMD% /M:"%%i\%%j"

                %ENCODER% %ARGV%

                rem ���z�h���C�u����A���}�E���g
                %UNMOUNT_CMD%
            )
        )
        if exist purchased (
            echo [#]Purchased digital data  : %%i
        )
        rem *.cue �t�@�C���������A�I�[�f�B�I�t�@�C���݂̂����c���Ă��Ȃ��ꍇ�̓f�B���N�g�������M���O����
        if not "%LEGACY_FILE_SUFFIX1%" == "" (
            if exist *.%LEGACY_FILE_SUFFIX1% (
                echo [!]LEGACY AUDIO FILE FOUND : %%i
            )
        )
        if not "%LEGACY_FILE_SUFFIX2%" == "" (
            if exist *.%LEGACY_FILE_SUFFIX2% (
                echo [!]LEGACY AUDIO FILE FOUND : %%i
            )
        )
        @echo;

    popd
)

goto finally

:usage
echo;
echo usage
echo  auto-cd-encoder.bat *.properties cddrive cd-image-directory

goto finally

:finally

endlocal
