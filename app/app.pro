include(../defaults.pri)

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage -O0
#    LIBS += -lgcov
    QMAKE_LFLAGS += -coverage
}

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
#CONFIG -= qt

SOURCES += main.cpp

LIBS += -L../src -lMusicOnDemand
