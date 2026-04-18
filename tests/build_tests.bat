@echo off
REM 测试项目编译脚本
REM 使用方法: build_tests.bat [xcp|common|all]

setlocal enabledelayedexpansion

set "TEST_ROOT=%~dp0"
set "QTDIR=D:\Qt\Qt5.9.9\5.9.9\mingw53_32"
set "QTBIN=%QTDIR%\bin"
set "MINGW=D:\Qt\Qt5.9.9\Tools\mingw530_32\bin"
set "PATH=%QTBIN%;%MINGW%;%PATH%"

IF "%1"=="" GOTO :all
IF "%1"=="all" GOTO :all
IF "%1"=="xcp" GOTO :xcp
IF "%1"=="common" GOTO :common
GOTO :usage

:xcp
echo ===== 编译 XCP_COMM 测试 =====
cd /d "%TEST_ROOT%xcp_comm_tests"
qmake xcp_comm_tests.pro -spec win32-g++ CONFIG+=release
if errorlevel 1 (
    echo ERROR: qmake failed
    exit /b 1
)
mingw32-make -j4 release
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)
echo ===== 运行 XCP_COMM 测试 =====
release\tst_xcpcomm.exe
GOTO :end

:common
echo ===== 编译 Common 模块测试 =====
cd /d "%TEST_ROOT%xcp_comm_tests"
qmake tst_common_modules.pro -spec win32-g++ CONFIG+=release
if errorlevel 1 (
    echo ERROR: qmake failed
    exit /b 1
)
mingw32-make -j4 release
if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)
echo ===== 运行 Common 模块测试 =====
release\tst_common_modules.exe
GOTO :end

:all
call :xcp
call :common
GOTO :end

:usage
echo 使用方法: build_tests.bat [xcp^|common^|all]
GOTO :end

:end
cd /d "%TEST_ROOT%"
echo ===== 完成 =====
endlocal
