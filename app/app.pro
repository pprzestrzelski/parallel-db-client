TARGET = db_client_test
TEMPLATE = app

QT += core sql network
QT -= gui

CONFIG += console c++11
CONFIG -= app_bundle
DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    
SOURCES = \
    main.cpp

INCLUDEPATH += $$PWD/../lib
LIBS += -L$$OUT_PWD/../lib -lparalleldbclient

#unix {
#    target.path = PATH_TO_INSTALL
#    INSTALLS += target
#}
