#-------------------------------------------------
#
# Project created by QtCreator 2020-07-05T08:18:38
#
#-------------------------------------------------

QT       += core gui xml sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WT5000Commu
TEMPLATE = lib
CONFIG += plugin

DEFINES += WT5000COMMU_LIBRARY
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        mainwindow.cpp \
    wt5000commu.cpp \
    wt5000thread.cpp

HEADERS += \
        mainwindow.h \
    driverinterface.h \
    wt5000commu.h \
    wt5000thread.h

FORMS += \
        mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/vc/ -ltmctl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/vc/ -ltmctld

INCLUDEPATH += $$PWD/vc
DEPENDPATH += $$PWD/vc

#静态调用FN_DataShare Dll

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../release/function/ -ldatatransfer
INCLUDEPATH += $$PWD/../datatransfer
DEPENDPATH += $$PWD/../datatransfer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../release/function/ -lFN_DataShare
INCLUDEPATH += $$PWD/../FN_DataShare
DEPENDPATH += $$PWD/../FN_DataShare

win32 {
    CONFIG(debug, release|debug):DESTDIR = ../debug/plugins/
    CONFIG(release, release|debug):DESTDIR = ../release/plugins/
} else {
    DESTDIR = ../plugins/
}
