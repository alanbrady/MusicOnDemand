include(../defaults.pri)

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage -O0
    QMAKE_LFLAGS += -coverage
}

Debug:DESTDIR = ./
Release:DESTDIR = ./

TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle

QT += testlib

LIBS += -L../src -lMusicOnDemand

SOURCES += \
    test_serverinterface.cpp \
    test_mediadataserver.cpp \
    main.cpp

HEADERS += \
    mockclient.h \
    test_mediadataserver.h \
    test_serverinterface.h

