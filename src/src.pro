include(../defaults.pri)

CONFIG += qt
TARGET = MusicOnDemand
TEMPLATE = lib

Debug:DESTDIR = ./
Release:DESTDIR = ./

HEADERS += \
    audiodataserver.h \
    mainwindow.h \
    tdd.h \
    mediadataserver.h \
    musiclibrary.h \
    musiclibrarythread.h \
    managelibrarydialog.h \
    tag.h \
    id3tagparser.h \
    serverinterface.h \
    serverfactory.h \
    socketlist.h

SOURCES += \
    audiodataserver.cpp \
    mainwindow.cpp \
    mediadataserver.cpp \
    musiclibrary.cpp \
    id3taginterface.cpp \
    musiclibrarythread.cpp \
    managelibrarydialog.cpp \
    tag.cpp \
    id3tagparser.cpp \
    serverinterface.cpp \
    serverfactory.cpp \
    socketlist.cpp
	
FORMS    += mainwindow.ui \
    managelibrarydialog.ui
