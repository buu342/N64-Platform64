@echo off
set LIBNAME=Engine Datatypes Library
echo Compiling %LIBNAME%


:Setup
set ROOT=C:\ultra
set PROJDIR=..\..\


:Don't touch
set gccdir=%ROOT%\gcc
set PATH=%ROOT%\gcc\mipse\bin;%ROOT%\usr\sbin;C:\WINDOWS\system32;
set gccsw=-mips3 -mgp32 -mfp32 -funsigned-char -D_LANGUAGE_C -D_ULTRA64 -D__EXTENSIONS__
set n64align=on
set DEBUG_MODE=0
for %%I in (.) do set CURRDIRNAME=%%~nxI
goto CheckDebugMode


:CheckDebugMode
for /f "tokens=3" %%a in ('find /i "DEBUG_MODE" %PROJDIR%debug.h') do if not defined debugcheck set "debugcheck=%%a"
if "%debugcheck%"=="1" (
    if "%SILENT%"=="" (
        echo Debug mode enabled
    )
    set DEBUG_MODE=1
) else if "%debugcheck%"=="0" (
    if "%SILENT%"=="" (
        echo Debug mode disabled
    )
) else (
    if "%SILENT%"=="" (
        echo Unable to retrieve debug mode
    )
)
echo.
goto CheckDirectories


:CheckDirectories
if not exist %ROOT% (
    echo ERROR: Unable to find ultra64 folder.
    goto Finish
)
goto MoveFromOut


:MoveFromOut
if not exist out goto make
cd out >nul 2>&1
move *.o .. >nul 2>&1
move *.d .. >nul 2>&1
move *.a .. >nul 2>&1
cd..
goto Make


:Make
make
set MAKEERROR=%errorlevel%
echo.
goto ErrorCheck


:ErrorCheck
if "%MAKEERROR%"=="2" (
    echo An error occurred during compilation.
)
goto MoveToOut


:MoveToOut
md out >nul 2>&1
move *.o out >nul 2>&1
move *.d out >nul 2>&1
move *.a out >nul 2>&1
md %PROJDIR%out >nul 2>&1
copy out\*.a %PROJDIR%out  >nul 2>&1
if "%MAKEERROR%"=="0" (
	echo %LIBNAME% compiled sucessfully!
)
goto Finish


:Finish
echo.
if "%SILENT%"=="" (
    pause
    exit
)
if "%MAKEERROR%"=="2" (
    pause
    exit
)