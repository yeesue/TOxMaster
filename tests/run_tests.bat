@echo off
REM 测试运行脚本
REM 使用方法: run_tests.bat [xcp|common|all]
REM 注意: 需先运行 build_tests.bat 编译测试

setlocal enabledelayedexpansion

set "TEST_ROOT=%~dp0"
set "QTDIR=D:\Qt\Qt5.9.9\5.9.9\mingw53_32"
set "QTBIN=%QTDIR%\bin"
set "MINGW=D:\Qt\Qt5.9.9\Tools\mingw530_32\bin"
set "PATH=%QTBIN%;%MINGW%;%PATH%"

set "TEST_FAILED=0"

IF "%1"=="" GOTO :all
IF "%1"=="all" GOTO :all
IF "%1"=="xcp" GOTO :xcp
IF "%1"=="common" GOTO :common
GOTO :usage

:xcp
echo ===== 运行 XCP_COMM 测试 =====
cd /d "%TEST_ROOT%xcp_comm_tests"
if not exist "release\tst_xcpcomm.exe" (
    echo ERROR: tst_xcpcomm.exe not found. Run build_tests.bat first.
    set "TEST_FAILED=1"
    GOTO :end
)
release\tst_xcpcomm.exe
if errorlevel 1 set "TEST_FAILED=1"
GOTO :end

:common
echo ===== 运行 Common 模块测试 =====
cd /d "%TEST_ROOT%xcp_comm_tests"
if not exist "release\tst_common_modules.exe" (
    echo ERROR: tst_common_modules.exe not found. Run build_tests.bat first.
    set "TEST_FAILED=1"
    GOTO :end
)
release\tst_common_modules.exe
if errorlevel 1 set "TEST_FAILED=1"
GOTO :end

:all
call :xcp
call :common
GOTO :end

:usage
echo 使用方法: run_tests.bat [xcp^|common^|all]
GOTO :end

:end
cd /d "%TEST_ROOT%"
if "%TEST_FAILED%"=="1" (
    echo ===== 部分测试失败 =====
    endlocal
    exit /b 1
) else (
    echo ===== 全部测试通过 =====
    endlocal
)
