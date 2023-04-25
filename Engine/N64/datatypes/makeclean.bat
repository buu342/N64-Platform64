@echo off

set FORCECLEAN=0

:: Request cleanup
if "%FORCECLEAN%"=="1" goto Cleanup
if not "%SILENT%"=="" goto Cleanup
set /P PROMPT=Clean the project directory[Y/N]?
if /I "%PROMPT%" EQU "N" goto Finish 
if /I "%PROMPT%" EQU "Y" (
    echo. 
    goto Cleanup
)

:: Remove the files
:Cleanup
if "%SILENT%"=="" (
    echo Cleaning up the libary directory
)
del /q *.o 2>nul
del /q *.d 2>nul
del /q *.a 2>nul
rmdir /s /q out 2>nul
echo Done!
goto Finish

:: Finished
:Finish
echo.
if "%SILENT%"=="" (
    pause
    exit
)