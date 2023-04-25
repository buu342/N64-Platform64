@echo off

set FORCECLEAN=0

:: Request cleanup
if "%FORCECLEAN%"=="1" goto HandleDependencies
set /P PROMPT=Clean the project directory[Y/N]?
if /I "%PROMPT%" EQU "N" goto Finish 
if /I "%PROMPT%" EQU "Y" (
    echo. 
    goto HandleDependencies
)

:HandleDependencies
echo Cleaning up the project directory
echo Cleaning dependencies
for /r %%F in ("makeclean.bat") do (
    if /i not "%~dp0"=="%%~dpF" (
        if exist "%%~F" (
            cd "%%~dpF"
            set SILENT=1
            call "%%~F"
            cd "%~dp0"
        )
    )
)
goto :Cleanup

:: Remove the files
:Cleanup
echo Cleaning main
del /q *.o 2>nul
del /q *.d 2>nul
del /q *.a 2>nul
del /q *.out 2>nul
del /q *.n64 2>nul
del /q disassembly.txt 2>nul
rmdir /s /q out 2>nul
echo Done!
goto Finish

:: Finished
:Finish
echo.
pause
exit