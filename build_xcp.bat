@echo off
setlocal

set "PROJECT_ROOT=%~dp0"
set "XCP_SRC=%PROJECT_ROOT%XCP_COMM"

set "QTDIR=D:\Qt\Qt5.9.9\5.9.9\mingw53_32"
set "QTBIN=%QTDIR%\bin"
set "MINGW=D:\Qt\Qt5.9.9\Tools\mingw530_32\bin"
set "PATH=%QTBIN%;%MINGW%;%PATH%"

cd /d "%XCP_SRC%"

REM Clean previous build artifacts
if exist build\obj rmdir /s /q build\obj
if exist build\moc rmdir /s /q build\moc
if exist build\ui rmdir /s /q build\ui
if exist build\rcc rmdir /s /q build\rcc
if exist Makefile del Makefile
if exist Makefile.Release del Makefile.Release
if exist Makefile.Debug del Makefile.Debug

echo Running qmake...
qmake XCP_COMM.pro -spec win32-g++ CONFIG+=release
if errorlevel 1 (
    echo qmake failed!
    exit /b 1
)

echo Building...
mingw32-make -j4
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo Done!
