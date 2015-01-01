#include <QApplication>
#include <QtTest/QtTest>

#include "test_serverinterface.h"
#include "test_mediadataserver.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Test_ServerInterface test_serverInterface;
    QTest::qExec(&test_serverInterface);

    Test_MediaDataServer test_mediaDataServer;
    QTest::qExec(&test_mediaDataServer);

    return 0;
}
