@echo off
REM ============================================================
REM Build A2L_Parser DLL and copy to XCP_COMM
REM ============================================================

setlocal

set "PROJECT_ROOT=%~dp0"
set "A2L_SRC=%PROJECT_ROOT%A2L_Praser"
set "XCP_DRIVER=%PROJECT_ROOT%XCP_COMM\build\driver"

REM Qt installation path
set "QTDIR=D:\Qt\Qt5.9.9\5.9.9\mingw53_32"
set "QTBIN=%QTDIR%\bin"
set "MINGW=D:\Qt\Qt5.9.9\Tools\mingw530_32\bin"
set "PATH=%QTBIN%;%MINGW%;%PATH%"

echo ============================================================
echo Building A2L_Parser DLL
echo ============================================================
echo.

REM Create driver directory
if not exist "%XCP_DRIVER%" mkdir "%XCP_DRIVER%"

cd /d "%A2L_SRC%"

echo [1/3] Running qmake...
qmake A2L_Parser_DLL.pro -spec win32-g++ CONFIG+=release
if errorlevel 1 (
    echo ERROR: qmake failed
    exit /b 1
)

echo.
echo [2/3] Building DLL...
mingw32-make -j4 release
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

echo.
echo [3/3] Verifying output...
if exist "%XCP_DRIVER%\A2L_Parser.dll" (
    echo     A2L_Parser.dll - OK
    dir "%XCP_DRIVER%\A2L_Parser.dll"
) else (
    echo ERROR: A2L_Parser.dll not found in output directory
    exit /b 1
)

if exist "%XCP_DRIVER%\libA2L_Parser.a" (
    echo     libA2L_Parser.a - OK
)

echo.
echo ============================================================
echo Build successful!
echo DLL location: %XCP_DRIVER%\A2L_Parser.dll
echo ============================================================
cd /d "%PROJECT_ROOT%"
endlocal
