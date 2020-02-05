@echo off

rem ---------- to auto-cd-encoder.bat args -------------
rem %1 profile
rem %2 cddrive
rem %3 cd-image root
rem ----------------------------------------------------

setlocal

call auto-cd-encoder.bat #single.properties X "T:\User\CD-Image"

endlocal

