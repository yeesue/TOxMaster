QT += testlib
QT -= gui

CONFIG += qt console warn_on testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_xcpcomm

CONFIG += c++11

XCP_SRC = $$PWD/../../XCP_COMM

INCLUDEPATH += $$XCP_SRC \
               $$XCP_SRC/common \
               $$XCP_SRC/xcp \
               $$XCP_SRC/drivers/interfaces \
               $$PWD/../test_files

DEFINES += TEST_FILES_DIR=\\\"$$PWD/../test_files/\\\"

SOURCES += \
    tst_xcpcomm.cpp \
    $$XCP_SRC/xcp/ratfunc_converter.cpp \
    $$XCP_SRC/common/logger.cpp

HEADERS += \
    $$XCP_SRC/xcp/ratfunc_converter.h \
    $$XCP_SRC/common/error.h \
    $$XCP_SRC/common/logger.h \
    $$PWD/../test_files/can_driver_interface.h \
    $$PWD/../test_files/mock_can_driver.h \
    $$XCP_SRC/drivers/interfaces/can_types.h \
    $$XCP_SRC/drivers/interfaces/can_driver.h
