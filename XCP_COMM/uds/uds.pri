#****************************************
# uds module
#****************************************

HEADERS += \
    $$PWD/NIDiagCS.h \
    $$PWD/ni_uds_worker.h \
    $$PWD/nidiagcs_epilog.h \
    $$PWD/nidiagcs_extcode.h \
    $$PWD/nidiagcs_fundtypes.h \
    $$PWD/nidiagcs_platdefines.h \
    $$PWD/nidiagcs_prolog.h \
    $$PWD/uds_config.h \
    $$PWD/uds_fun_thread.h \
    $$PWD/uds_win.h \
    $$PWD/udssetdlg.h

SOURCES += \
    $$PWD/ni_uds_worker.cpp \
    $$PWD/uds_fun_thread.cpp \
    $$PWD/uds_win.cpp \
    $$PWD/udssetdlg.cpp

FORMS += \
    $$PWD/uds_win.ui \
    $$PWD/udssetdlg.ui


unix|win32: LIBS += -L$$PWD/ -lNIDiagCS

