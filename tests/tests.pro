include(../defaults.pri)

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
