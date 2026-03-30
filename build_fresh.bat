@echo off
setlocal

set "PROJECT_ROOT=%~dp0"
set "A2L_SRC=%PROJECT_ROOT%A2L_Praser"

set "QTDIR=D:\Qt\Qt5.9.9\5.9.9\mingw53_32"
set "QTBIN=%QTDIR%\bin"
set "MINGW=D:\Qt\Qt5.9.9\Tools\mingw530_32\bin"
set "PATH=%QTBIN%;%MINGW%;%PATH%"

cd /d "%A2L_SRC%"

REM Clean everything
if exist release rmdir /s /q release
if exist Makefile del Makefile
if exist Makefile.Release del Makefile.Release
if exist Makefile.Debug del Makefile.Debug

echo Running qmake...
qmake A2L_Parser_DLL.pro -spec win32-g++ CONFIG+=release
if errorlevel 1 exit /b 1

echo Building...
mingw32-make -j4
if errorlevel 1 exit /b 1

echo Done!
