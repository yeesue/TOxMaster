#****************************************
# wt5000 module
#****************************************

FORMS += \
    $$PWD/win_wt5000.ui \
    $$PWD/wt5000setdlg.ui

HEADERS += \
    $$PWD/vc/tmctl.h \
    $$PWD/win_wt5000.h \
    $$PWD/wt5000_thread.h \
    $$PWD/wt5000setdlg.h \
    $$PWD/wt5000thread.h

SOURCES += \
    $$PWD/win_wt5000.cpp \
    $$PWD/wt5000_thread.cpp \
    $$PWD/wt5000setdlg.cpp \
    $$PWD/wt5000thread.cpp


