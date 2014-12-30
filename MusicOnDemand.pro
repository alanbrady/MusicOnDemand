TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
	src \
	app \
	tests
app.depends = src
tests.depends = src

# needed for gcov
CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -fprofile-arcs
    QMAKE_CXXFLAGS += -ftest-coverage
}
