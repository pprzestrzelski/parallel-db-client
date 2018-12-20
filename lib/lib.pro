TARGET = paralleldbclient
TEMPLATE = lib

QT -= gui
QT += network sql

CONFIG += c++11

DEFINES += \
    LIB_LIBRARY \
    QT_DEPRECATED_WARNINGS

QMAKE_LFLAGS += -Wl,-z,defs

SOURCES += \
    paralleldbclient.cpp \
    dbconfig.cpp \
    paralleldbfactory.cpp \
    paralleldbmetainfo.cpp \
    utils.cpp

HEADERS += \
    paralleldbclient.h \
    dbconfig.h \
    constants.h \
    paralleldbfactory.h \
    paralleldbmetainfo.h \
    utils.h

unix {
    LIBS += -lodbc
    headers.files = paralleldbfactory.h paralleldbclient.h dbconfig.h
    headers.path = /usr/include
    target.path = /usr/lib
    INSTALLS += target headers
}

#win32 {
#}
