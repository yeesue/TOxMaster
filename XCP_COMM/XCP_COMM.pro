#-------------------------------------------------
#
# Project created by QtCreator 2020-08-27T23:02:33
#
#-------------------------------------------------

QT       += core gui xml axcontainer sql script printsupport concurrent datavisualization

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console
CONFIG -= ltcg  # Disable Link Time Code Generation to avoid liblto_plugin error

# ���ø��ϸ�ı��뾯��
QMAKE_CXXFLAGS += -Wall -Wextra


#TCM(Testing Communication Master)
TARGET = TCM
TEMPLATE = app


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# ָ��Ŀ���ļ����·��
win32:DESTDIR = $$PWD/build

# ָ������moc�������м��ļ����õ�Ŀ¼����Q_OBJECT���ͷ�ļ�ת���ɱ�׼.h�ļ��Ĵ��Ŀ¼��
win32:MOC_DIR = $$PWD/build/moc
unix: MOC_DIR = $$PWD/build/moc

# ָ�������м��ļ�(.o, .obj)���õ�Ŀ¼
win32: OBJECTS_DIR = $$PWD/build/obj
unix: OBJECTS_DIR = $$PWD/build/obj

# ָ��Qt��Դ����������ļ���Ŀ¼��.qrc�ļ�ת����qrc_*.h�ļ��Ĵ��Ŀ¼��
win32: RCC_DIR = $$PWD/build/rcc
unix: RCC_DIR = $$PWD/build/rcc

# ָ������uic�������м��ļ����õ�Ŀ¼��.ui�ļ�ת����ui_*.h�ļ��Ĵ��Ŀ¼��
win32: UI_DIR = $$PWD/build/ui
unix: UI_DIR = $$PWD/build/ui

# ָ�������û����淭���ı��ķ���(.ts)�ļ��б�
TRANSLATIONS += $$PWD/build/rcc/TCM_zh.ts \
                $$PWD/build/rcc/TCM_en.ts

# �鿴���ò���������
message($$PWD)

SOURCES += main.cpp\

HEADERS  += \

FORMS    += \

RESOURCES += \
    icon.qrc \
    icons.qrc

DISTFILES += \
    nicanmsc.lib \
    nixnet.lib

include($$PWD/workspace/workspace.pri)
include($$PWD/function/function.pri)
include($$PWD/mdf/mdf.pri)
include($$PWD/log/log.pri)
include($$PWD/pam/pam.pri)
include($$PWD/can/can.pri)
include($$PWD/can_ts/can_ts.pri)
include($$PWD/can_zlg/can_zlg.pri)
include($$PWD/drivers/drivers.pri)
include($$PWD/a2l/a2l.pri)
include($$PWD/xcp/xcp.pri)
include($$PWD/xcp_tester/xcp_tester.pri)
include($$PWD/script/script.pri)
include($$PWD/common/common.pri)
include($$PWD/excel/excel.pri)
include($$PWD/csv/csv.pri)
include($$PWD/win/win.pri)
#include($$PWD/uds/uds.pri)
include($$PWD/inca_com/inca_com.pri)
include($$PWD/docking/docking.pri)
include($$PWD/wt5000/wt5000.pri)
include($$PWD/wt3000/wt3000.pri)
include($$PWD/auto/auto.pri)

INCLUDEPATH += \
    $$PWD/workspace \
    $$PWD/function \
    $$PWD/log \
    $$PWD/pam \
    $$PWD/mdf \
    $$PWD/can \
    $$PWD/can_ts \
    $$PWD/can_zlg \
    $$PWD/a2l \
    $$PWD/xcp \
    $$PWD/xcp_tester \
    $$PWD/script \
    $$PWD/auto \
    $$PWD/common \
    $$PWD/excel \
    $$PWD/csv \
    $$PWD/win \
    $$PWD/uds \
    $$PWD/inca_com \
    $$PWD/wt3000 \
    $$PWD/wt5000 \
    $$PWD/docking \

win32: LIBS += -L$$PWD/./ -lnixnet
win32: LIBS += -L$$PWD/./ -lnicanmsc

# A2L_Parser DLL import library
win32: LIBS += -L$$PWD/build/driver/ -lA2L_Parser
INCLUDEPATH += $$PWD/build/driver
DEPENDPATH += $$PWD/build/driver

RC_ICONS = AppIcon.ico

#DEFINES += QCUSTOMPLOT_USE_OPENG

# MinGW: enable stdcall fixup for MSVC libs
win32-g++: QMAKE_LFLAGS += -Wl,--enable-stdcall-fixup
win32: LIBS += -L$$PWD/can_ts/lib/windows/x86/ -llibTSCAN

INCLUDEPATH += $$PWD/can_ts/lib/windows/x86
DEPENDPATH += $$PWD/can_ts/lib/windows/x86

win32: LIBS += -L$$PWD/can_zlg/zlgcan_x86/ -llibzlgcan

INCLUDEPATH += $$PWD/can_zlg/zlgcan_x86
DEPENDPATH += $$PWD/can_zlg/zlgcan_x86


win32: LIBS += -L$$PWD/wt5000/vc/ -llibtmctl

INCLUDEPATH += $$PWD/wt5000/vc
DEPENDPATH += $$PWD/wt5000/vc

win32: LIBS += -L$$PWD/wt3000/vc/ -llibtmctl

INCLUDEPATH += $$PWD/wt3000/vc
DEPENDPATH += $$PWD/wt3000/vc
