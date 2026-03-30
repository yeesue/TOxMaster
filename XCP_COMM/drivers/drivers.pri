# drivers.pri - CAN driver module

# Interface files (always included)
HEADERS += \
    $$PWD/interfaces/can_types.h \
    $$PWD/interfaces/can_driver.h

SOURCES += \
    $$PWD/interfaces/can_driver_factory.cpp

# NI-XNET driver
contains(DEFINES, USE_NI_XNET) {
    HEADERS += $$PWD/ni/ni_can_driver.h
    SOURCES += $$PWD/ni/ni_can_driver.cpp
    INCLUDEPATH += $$PWD/ni
    LIBS += -lnixnet
}

# TSCAN driver
contains(DEFINES, USE_TSCAN) {
    HEADERS += $$PWD/ts/ts_can_driver.h
    SOURCES += $$PWD/ts/ts_can_driver.cpp
    INCLUDEPATH += $$PWD/ts
    LIBS += -lTSCAN
}

# ZLG driver
contains(DEFINES, USE_ZLG) {
    HEADERS += $$PWD/zlg/zlg_can_driver.h
    SOURCES += $$PWD/zlg/zlg_can_driver.cpp
    INCLUDEPATH += $$PWD/zlg
    LIBS += -lzlgcan
}

# Default: include all drivers (backward compatibility)
!contains(DEFINES, USE_NI_XNET):!contains(DEFINES, USE_TSCAN):!contains(DEFINES, USE_ZLG) {
    HEADERS += \
        $$PWD/ni/ni_can_driver.h \
        $$PWD/ts/ts_can_driver.h \
        $$PWD/zlg/zlg_can_driver.h
    
    SOURCES += \
        $$PWD/ni/ni_can_driver.cpp \
        $$PWD/ts/ts_can_driver.cpp \
        $$PWD/zlg/zlg_can_driver.cpp
}

# Public include path
INCLUDEPATH += $$PWD/interfaces
