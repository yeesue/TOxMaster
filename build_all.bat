@echo off
REM ============================================================
REM TOxMaster Build Script
REM Usage: build_all.bat [option]
REM   (no args)  - Full build: DLL + XCP_COMM + Deploy
REM   dll        - Build A2L_Parser DLL only
REM   xcp        - Build XCP_COMM only
REM   deploy     - Deploy runtime dependencies only
REM   clean      - Clean all build artifacts
REM   fresh      - Clean + Full build
REM ============================================================

setlocal enabledelayedexpansion

set "PROJECT_ROOT=%~dp0"
set "A2L_SRC=%PROJECT_ROOT%A2L_Parser"
set "XCP_SRC=%PROJECT_ROOT%XCP_COMM"
set "XCP_BUILD=%XCP_SRC%\build"
set "XCP_DRIVER=%XCP_BUILD%\driver"

REM Qt installation path
set "QTDIR=D:\Qt\Qt5.9.9\5.9.9\mingw53_32"
set "QTBIN=%QTDIR%\bin"
set "MINGW=D:\Qt\Qt5.9.9\Tools\mingw530_32\bin"
set "PATH=%QTBIN%;%MINGW%;%PATH%"

REM Parse argument
set "BUILD_MODE=%1"
if "%BUILD_MODE%"=="" set "BUILD_MODE=all"

REM ============================================================
REM Main Logic
REM ============================================================

if /i "%BUILD_MODE%"=="clean" goto :do_clean
if /i "%BUILD_MODE%"=="fresh" goto :do_fresh
if /i "%BUILD_MODE%"=="dll" goto :do_dll
if /i "%BUILD_MODE%"=="xcp" goto :do_xcp
if /i "%BUILD_MODE%"=="deploy" goto :do_deploy
if /i "%BUILD_MODE%"=="all" goto :do_all

echo Unknown option: %BUILD_MODE%
echo Usage: build_all.bat [dll^|xcp^|deploy^|clean^|fresh^|all]
exit /b 1

REM ============================================================
REM Clean Build Artifacts
REM ============================================================
:do_clean
echo ============================================================
echo Cleaning all build artifacts...
echo ============================================================

REM Clean A2L_Parser
if exist "%A2L_SRC%\release" rmdir /s /q "%A2L_SRC%\release"
if exist "%A2L_SRC%\Makefile" del "%A2L_SRC%\Makefile"
if exist "%A2L_SRC%\Makefile.Release" del "%A2L_SRC%\Makefile.Release"
if exist "%A2L_SRC%\Makefile.Debug" del "%A2L_SRC%\Makefile.Debug"

REM Clean XCP_COMM
if exist "%XCP_BUILD%\obj" rmdir /s /q "%XCP_BUILD%\obj"
if exist "%XCP_BUILD%\moc" rmdir /s /q "%XCP_BUILD%\moc"
if exist "%XCP_BUILD%\ui" rmdir /s /q "%XCP_BUILD%\ui"
if exist "%XCP_BUILD%\rcc" rmdir /s /q "%XCP_BUILD%\rcc"
if exist "%XCP_SRC%\Makefile" del "%XCP_SRC%\Makefile"
if exist "%XCP_SRC%\Makefile.Release" del "%XCP_SRC%\Makefile.Release"
if exist "%XCP_SRC%\Makefile.Debug" del "%XCP_SRC%\Makefile.Debug"

echo Clean complete!
goto :end

REM ============================================================
REM Fresh Build (Clean + All)
REM ============================================================
:do_fresh
call :do_clean
call :do_all
goto :end

REM ============================================================
REM Full Build
REM ============================================================
:do_all
echo ============================================================
echo [1/3] Building A2L_Parser DLL
echo ============================================================
call :do_dll
if errorlevel 1 exit /b 1

echo.
echo ============================================================
echo [2/3] Building XCP_COMM
echo ============================================================
call :do_xcp
if errorlevel 1 exit /b 1

echo.
echo ============================================================
echo [3/3] Deploying runtime dependencies
echo ============================================================
call :do_deploy

echo.
echo ============================================================
echo Build complete!
echo Executable: %XCP_BUILD%\TCM.exe
echo ============================================================
goto :end

REM ============================================================
REM Build A2L_Parser DLL
REM ============================================================
:do_dll
if not exist "%XCP_DRIVER%" mkdir "%XCP_DRIVER%"

cd /d "%A2L_SRC%"

echo Running qmake...
qmake A2L_Parser_DLL.pro -spec win32-g++ CONFIG+=release
if errorlevel 1 (
    echo ERROR: qmake failed
    exit /b 1
)

echo Building DLL...
mingw32-make -j4 release
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

echo Verifying output...
if exist "%XCP_DRIVER%\A2L_Parser.dll" (
    echo     A2L_Parser.dll - OK
) else (
    echo ERROR: A2L_Parser.dll not found
    exit /b 1
)

cd /d "%PROJECT_ROOT%"
goto :eof

REM ============================================================
REM Build XCP_COMM
REM ============================================================
:do_xcp
cd /d "%XCP_SRC%"

echo Running qmake...
qmake XCP_COMM.pro -spec win32-g++ CONFIG+=release
if errorlevel 1 (
    echo ERROR: qmake failed
    exit /b 1
)

echo Building...
mingw32-make -j4 release
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

cd /d "%PROJECT_ROOT%"
goto :eof

REM ============================================================
REM Deploy Runtime Dependencies
REM ============================================================
:do_deploy
echo Deploying runtime dependencies...

REM Create driver directory if not exists
if not exist "%XCP_DRIVER%" mkdir "%XCP_DRIVER%"

echo [1/4] Copying Qt libraries...
for %%L in (
    Qt5Core.dll Qt5Gui.dll Qt5Widgets.dll Qt5Xml.dll
    Qt5Network.dll Qt5Sql.dll Qt5PrintSupport.dll Qt5Concurrent.dll
    Qt5Script.dll Qt5AxContainer.dll Qt5AxBase.dll
) do (
    if exist "%QTBIN%\%%L" (
        copy /Y "%QTBIN%\%%L" "%XCP_BUILD%\" >nul
        echo     %%L
    )
)

echo [2/4] Copying MinGW runtime...
for %%L in (libgcc_s_dw2-1.dll libstdc++-6.dll libwinpthread-1.dll) do (
    if exist "%MINGW%\%%L" (
        copy /Y "%MINGW%\%%L" "%XCP_BUILD%\" >nul
        echo     %%L
    )
)

echo [3/4] Copying Qt plugins...
set "QTPLUGINS=%QTDIR%\plugins"
if exist "%QTPLUGINS%" (
    if not exist "%XCP_BUILD%\platforms" mkdir "%XCP_BUILD%\platforms"
    if not exist "%XCP_BUILD%\imageformats" mkdir "%XCP_BUILD%\imageformats"
    if not exist "%XCP_BUILD%\styles" mkdir "%XCP_BUILD%\styles"
    if not exist "%XCP_BUILD%\printsupport" mkdir "%XCP_BUILD%\printsupport"
    
    xcopy /Y /Q "%QTPLUGINS%\platforms\*.dll" "%XCP_BUILD%\platforms\" >nul 2>&1
    xcopy /Y /Q "%QTPLUGINS%\imageformats\*.dll" "%XCP_BUILD%\imageformats\" >nul 2>&1
    xcopy /Y /Q "%QTPLUGINS%\styles\*.dll" "%XCP_BUILD%\styles\" >nul 2>&1
    xcopy /Y /Q "%QTPLUGINS%\printsupport\*.dll" "%XCP_BUILD%\printsupport\" >nul 2>&1
    echo     Qt plugins copied
)

echo [4/4] Copying third-party drivers...
REM CAN drivers - TS
set "CAN_TS=%PROJECT_ROOT%XCP_COMM\can_ts\lib\windows\x86"
if exist "%CAN_TS%\libTSCAN.dll" (
    copy /Y "%CAN_TS%\libTSCAN.dll" "%XCP_DRIVER%\" >nul
    echo     libTSCAN.dll
)

REM CAN drivers - ZLG
set "CAN_ZLG=%PROJECT_ROOT%XCP_COMM\can_zlg\zlgcan_x86"
if exist "%CAN_ZLG%\zlgcan.dll" (
    copy /Y "%CAN_ZLG%\zlgcan.dll" "%XCP_BUILD%\" >nul
    echo     zlgcan.dll
    if exist "%CAN_ZLG%\zlgcan_wrap.dll" (
        copy /Y "%CAN_ZLG%\zlgcan_wrap.dll" "%XCP_BUILD%\" >nul
        echo     zlgcan_wrap.dll
    )
)

REM ZLG kernel dlls
if exist "%CAN_ZLG%\kerneldlls" (
    xcopy /Y /E /Q "%CAN_ZLG%\kerneldlls\*" "%XCP_BUILD%\kerneldlls\" >nul 2>&1
    echo     ZLG kerneldlls copied
)

REM WT libraries
set "WT5000=%PROJECT_ROOT%XCP_COMM\wt5000\vc"
if exist "%WT5000%\tmctl.dll" (
    copy /Y "%WT5000%\tmctl.dll" "%XCP_DRIVER%\" >nul
    echo     tmctl.dll
)

set "WT3000=%PROJECT_ROOT%XCP_COMM\wt3000\vc"
if exist "%WT3000%\tmctl.dll" (
    copy /Y "%WT3000%\tmctl.dll" "%XCP_DRIVER%\" >nul
    echo     tmctl.dll (WT3000)
)

echo Deployment complete!
goto :eof

REM ============================================================
REM End
REM ============================================================
:end
cd /d "%PROJECT_ROOT%"
endlocal
