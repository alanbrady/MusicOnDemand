include(../defaults.pri)

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage -O0
    LIBS += -lgcov
}

Debug:DESTDIR = ./
Release:DESTDIR = ./

TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle

QT += testlib

LIBS += -L../src -lMusicOnDemand

SOURCES += \
    test_serverinterface.cpp

#HEADERS += \
#    test_serverinterface.h
