#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T15:58:42
#
#-------------------------------------------------

#CONFIG(debug, debug|release): DEFINES += DEBUG

#INCLUDEPATH += ../taglib-1.6.3 \
#    ../taglib-1.6.3/taglib \
#    ../taglib-1.6.3/taglib/toolkit \
#    ../taglib-1.6.3/taglib/mpeg

#LIBS += ../taglib-1.6.3/taglib-release/libtaglib.a

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicOnDemandServer
TEMPLATE = app

HEADERS += \
    socketinterface.h \
    audiosocketthread.h \
    audiodataserver.h \
    mainwindow.h \
    tdd.h \
    mediadataserver.h \
    mediasocketthread.h \
    musiclibrary.h \
    id3taginterface.h \
    musiclibrarythread.h \
    managelibrarydialog.h \
    tag.h \
    id3tagparser.h

SOURCES += \
    socketinterface.cpp \
    audiosocketthread.cpp \
    audiodataserver.cpp \
    main.cpp\
    mainwindow.cpp \
    mediadataserver.cpp \
    mediasocketthread.cpp \
    musiclibrary.cpp \
    id3taginterface.cpp \
    musiclibrarythread.cpp \
    managelibrarydialog.cpp \
    tag.cpp \
    id3tagparser.cpp

FORMS    += mainwindow.ui \
    managelibrarydialog.ui















