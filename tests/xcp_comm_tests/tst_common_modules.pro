QT += testlib core concurrent
QT -= gui

CONFIG += qt console warn_on testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_common_modules

CONFIG += c++11

XCP_SRC = $$PWD/../../XCP_COMM

INCLUDEPATH += $$XCP_SRC \
               $$XCP_SRC/common \
               $$XCP_SRC/xcp \
               $$XCP_SRC/drivers/interfaces \
               $$PWD/../test_files

SOURCES += \
    tst_common_modules.cpp \
    $$XCP_SRC/common/config_manager.cpp \
    $$XCP_SRC/common/memory_manager.cpp \
    $$XCP_SRC/common/logger.cpp \
    $$XCP_SRC/common/thread_pool.cpp \
    $$XCP_SRC/common/thread_communicator.cpp \
    $$XCP_SRC/common/data_replay_manager.cpp \
    $$XCP_SRC/common/test_case_manager.cpp

HEADERS += \
    $$XCP_SRC/common/config_manager.h \
    $$XCP_SRC/common/memory_manager.h \
    $$XCP_SRC/common/logger.h \
    $$XCP_SRC/common/thread_pool.h \
    $$XCP_SRC/common/thread_communicator.h \
    $$XCP_SRC/common/smart_ptr.h \
    $$XCP_SRC/common/data_replay_manager.h \
    $$XCP_SRC/common/test_case_manager.h \
    $$XCP_SRC/common/error.h
