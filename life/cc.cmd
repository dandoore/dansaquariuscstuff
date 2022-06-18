@echo off
REM
REM Windows Batch file for launching ZCC to compile Aquarius executables and lauch them in Aqualite
set FILE=life
set EMUPATH=D:\Users\Dan\Documents\Aquarius\AquaLite\AquaLite.exe

echo Compiling %FILE%.c for ROM... 
zcc +aquarius %FILE%.c -subtype=rom -clib=ansi -lm -create-app -o %FILE%
IF NOT %ERRORLEVEL% == 0  GOTO FAIL
echo Compiling %FILE%.c for Tape... 
zcc +aquarius %FILE%.c -clib=ansi -lm -create-app -o %FILE%
IF NOT %ERRORLEVEL% == 0  GOTO FAIL

echo Removing binaries for %FILE%...
del %FILE% 1> nul 2> nul
IF NOT %ERRORLEVEL% == 0  GOTO FAIL
del %FILE%_BSS.bin 1> nul 2> nul
IF NOT %ERRORLEVEL% == 0  GOTO FAIL
del %FILE%_DATA.bin 1> nul 2> nul
IF NOT %ERRORLEVEL% == 0  GOTO FAIL

echo Launching ROM image %FILE% with Aqualite...
start /wait %EMUPATH% %CD%\%FILE%.rom
IF NOT %ERRORLEVEL% == 0  GOTO FAIL

GOTO END

:FAIL
Echo  ^^ %FILE% failed, press a key.
pause 1> nul 2> nul
:END