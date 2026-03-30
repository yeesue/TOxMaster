@echo off
REM ============================================================
REM TOxMaster Deployment Script
REM Copy necessary libraries to build directory
REM ============================================================

setlocal enabledelayedexpansion

REM Project paths
set "PROJECT_ROOT=%~dp0"
set "BUILD_DIR=%PROJECT_ROOT%XCP_COMM\build"

REM Qt installation path
set "QTDIR=D:\Qt\Qt5.9.9\5.9.9\mingw53_32"
set "QTBIN=%QTDIR%\bin"
set "MINGW=%QTDIR%\..\..\Tools\mingw530_32\bin"

echo ============================================================
echo Deploying runtime dependencies to build directory
echo ============================================================
echo.

REM Create driver directory if not exists
if not exist "%BUILD_DIR%\driver" mkdir "%BUILD_DIR%\driver"

echo [1/4] Copying Qt libraries...
for %%L in (
    Qt5Core.dll
    Qt5Gui.dll
    Qt5Widgets.dll
    Qt5Xml.dll
    Qt5Network.dll
    Qt5Sql.dll
    Qt5PrintSupport.dll
    Qt5Concurrent.dll
    Qt5Script.dll
    Qt5AxContainer.dll
    Qt5AxBase.dll
) do (
    if exist "%QTBIN%\%%L" (
        copy /Y "%QTBIN%\%%L" "%BUILD_DIR%\" >nul
        echo     %%L
    )
)

echo [2/4] Copying MinGW runtime...
for %%L in (
    libgcc_s_dw2-1.dll
    libstdc++-6.dll
    libwinpthread-1.dll
) do (
    if exist "%MINGW%\%%L" (
        copy /Y "%MINGW%\%%L" "%BUILD_DIR%\" >nul
        echo     %%L
    )
)

echo [3/4] Copying Qt plugins...
set "QTPLUGINS=%QTDIR%\plugins"
if exist "%QTPLUGINS%" (
    if not exist "%BUILD_DIR%\platforms" mkdir "%BUILD_DIR%\platforms"
    if not exist "%BUILD_DIR%\imageformats" mkdir "%BUILD_DIR%\imageformats"
    if not exist "%BUILD_DIR%\styles" mkdir "%BUILD_DIR%\styles"
    if not exist "%BUILD_DIR%\printsupport" mkdir "%BUILD_DIR%\printsupport"
    
    xcopy /Y /Q "%QTPLUGINS%\platforms\*.dll" "%BUILD_DIR%\platforms\" >nul 2>&1
    xcopy /Y /Q "%QTPLUGINS%\imageformats\*.dll" "%BUILD_DIR%\imageformats\" >nul 2>&1
    xcopy /Y /Q "%QTPLUGINS%\styles\*.dll" "%BUILD_DIR%\styles\" >nul 2>&1
    xcopy /Y /Q "%QTPLUGINS%\printsupport\*.dll" "%BUILD_DIR%\printsupport\" >nul 2>&1
    echo     Qt plugins copied
)

echo [4/4] Copying third-party drivers...
REM CAN drivers - TS
set "CAN_TS=%PROJECT_ROOT%XCP_COMM\can_ts\lib\windows\x86"
if exist "%CAN_TS%\libTSCAN.dll" (
    copy /Y "%CAN_TS%\libTSCAN.dll" "%BUILD_DIR%\driver\" >nul
    echo     libTSCAN.dll
)

REM CAN drivers - ZLG (check multiple locations)
set "CAN_ZLG=%PROJECT_ROOT%XCP_COMM\can_zlg\zlgcan_x86"
if exist "%CAN_ZLG%\zlgcan.dll" (
    copy /Y "%CAN_ZLG%\zlgcan.dll" "%BUILD_DIR%\" >nul
    echo     zlgcan.dll
    if exist "%CAN_ZLG%\zlgcan_wrap.dll" (
        copy /Y "%CAN_ZLG%\zlgcan_wrap.dll" "%BUILD_DIR%\" >nul
        echo     zlgcan_wrap.dll
    )
) else (
    echo     zlgcan.dll - NOT FOUND (may need to install ZLG CAN driver)
    echo     Expected location: %CAN_ZLG%\zlgcan.dll
)

REM Copy ZLG kernel dlls if available (required for zlgcan.dll to work)
if exist "%CAN_ZLG%\kerneldlls" (
    xcopy /Y /E /Q "%CAN_ZLG%\kerneldlls\*" "%BUILD_DIR%\kerneldlls\" >nul 2>&1
    echo     ZLG kerneldlls copied
)

REM WT libraries
set "WT5000=%PROJECT_ROOT%XCP_COMM\wt5000\vc"
if exist "%WT5000%\tmctl.dll" (
    copy /Y "%WT5000%\tmctl.dll" "%BUILD_DIR%\driver\" >nul
    echo     tmctl.dll
)

set "WT3000=%PROJECT_ROOT%XCP_COMM\wt3000\vc"
if exist "%WT3000%\tmctl.dll" (
    copy /Y "%WT3000%\tmctl.dll" "%BUILD_DIR%\driver\" >nul
    echo     tmctl.dll (WT3000)
)

echo.
echo ============================================================
echo Deployment complete!
echo Build directory: %BUILD_DIR%
echo.
echo Files:
dir /B "%BUILD_DIR%\*.dll" 2>nul
echo.
echo Driver files:
dir /B "%BUILD_DIR%\driver\*.dll" 2>nul
echo ============================================================
pause
