#-------------------------------------------------
# auto
#-------------------------------------------------

INCLUDEPATH     += $$PWD

HEADERS += \
    $$PWD/auto_pam.h \
    $$PWD/auto_struct.h \
    $$PWD/auto_win.h \
    $$PWD/autoexewin.h \
    $$PWD/cmd_dlg.h \
    $$PWD/sys_pam.h

SOURCES += \
    $$PWD/auto_pam.cpp \
    $$PWD/auto_win.cpp \
    $$PWD/autoexewin.cpp \
    $$PWD/cmd_dlg.cpp \
    $$PWD/sys_pam.cpp

FORMS += \
    $$PWD/auto_win.ui \
    $$PWD/autoexewin.ui \
    $$PWD/cmd_dlg.ui
