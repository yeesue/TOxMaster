HEADERS += \
    $$PWD/can_fun_thread_ts.h \
    $$PWD/can_thread_ts.h \
    $$PWD/dbcparser.h \
    $$PWD/lib/windows/libTSCAN.h \
    #$$PWD/lib/x86/TSCANDef.h \
 \    #$$PWD/lib/x86/TSCANLINApi.h
    $$PWD/tscansetdlg.h

SOURCES += \
    $$PWD/can_fun_thread_ts.cpp \
    $$PWD/can_thread_ts.cpp \
    $$PWD/dbcparser.cpp \
    $$PWD/tscansetdlg.cpp
     #$$PWD/lib/x86/TSCANLINApi.cpp

#INCLUDEPATH += $$PWD/lib/x86
#DEPENDPATH += $$PWD/lib/x86

FORMS += \
    $$PWD/tscansetdlg.ui
