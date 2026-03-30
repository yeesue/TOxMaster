@echo off
REM ============================================================
REM Build All: A2L_Parser DLL + XCP_COMM
REM ============================================================

setlocal

set "PROJECT_ROOT=%~dp0"
set "QTDIR=D:\Qt\Qt5.9.9\5.9.9\mingw53_32"
set "QTBIN=%QTDIR%\bin"
set "MINGW=D:\Qt\Qt5.9.9\Tools\mingw530_32\bin"
set "PATH=%QTBIN%;%MINGW%;%PATH%"

echo ============================================================
echo [1/2] Building A2L_Parser DLL
echo ============================================================
call "%PROJECT_ROOT%build_dll.bat"
if errorlevel 1 (
    echo ERROR: A2L_Parser DLL build failed
    exit /b 1
)

echo.
echo ============================================================
echo [2/2] Building XCP_COMM
echo ============================================================
cd /d "%PROJECT_ROOT%XCP_COMM"

echo Cleaning...
if exist Makefile mingw32-make clean 2>nul

echo Running qmake...
qmake XCP_COMM.pro -spec win32-g++ CONFIG+=release
if errorlevel 1 (
    echo ERROR: qmake failed
    exit /b 1
)

echo Building...
mingw32-make -j4 release
if errorlevel 1 (
    echo ERROR: XCP_COMM build failed
    exit /b 1
)

echo.
echo ============================================================
echo [3/3] Deploying runtime dependencies
echo ============================================================
call "%PROJECT_ROOT%deploy.bat"

echo.
echo ============================================================
echo Build complete!
echo Executable: %PROJECT_ROOT%XCP_COMM\build\TCM.exe
echo ============================================================

cd /d "%PROJECT_ROOT%"
endlocal
