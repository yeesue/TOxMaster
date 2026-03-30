QT += quick quickwidgets

lessThan(QT_MAJOR_VERSION, 6) {
    win32 {
        QT += axcontainer
    }
}

RESOURCES += \
    $$PWD/dock.qrc

FORMS += \
    $$PWD/StatusDialog.ui \
    $$PWD/dockwindow.ui

HEADERS += \
    $$PWD/DockWindow.h \
    $$PWD/ImageViewer.h \
    $$PWD/RenderWidget.h \
    $$PWD/StatusDialog.h

SOURCES += \
    $$PWD/DockWindow.cpp \
    $$PWD/ImageViewer.cpp \
    $$PWD/RenderWidget.cpp \
    $$PWD/StatusDialog.cpp

win32: LIBS += -L$$PWD/dockManage/ -lqtadvanceddocking

INCLUDEPATH += $$PWD/dockManage
DEPENDPATH += $$PWD/dockManage


