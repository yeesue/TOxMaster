QT += testlib
QT -= gui

CONFIG += qt console warn_on testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_xcpcomm

# 启用C++11
CONFIG += c++11

# 被测源文件路�?XCP_SRC = $$PWD/../../XCP_COMM

INCLUDEPATH += $$XCP_SRC \
               $$XCP_SRC/common \
               $$XCP_SRC/drivers/interfaces \
               $$PWD/../test_files

# 测试文件目录
DEFINES += TEST_FILES_DIR=\\\"$$PWD/../test_files/\\\"

# 测试源文�?SOURCES += \
    tst_xcpcomm.cpp \
    tst_ratfunc.cpp \
    $$XCP_SRC/xcp/ratfunc_converter.cpp

# 额外头文�?HEADERS += \
    $$XCP_SRC/xcp/ratfunc_converter.h

# 头文�?HEADERS += \
    $$PWD/../test_files/can_driver_interface.h \
    $$PWD/../test_files/mock_can_driver.h \
    $$XCP_SRC/common/error.h \
    $$XCP_SRC/drivers/interfaces/can_types.h \
    $$XCP_SRC/drivers/interfaces/can_driver.h
