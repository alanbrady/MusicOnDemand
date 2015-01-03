#include <QApplication>
#include <QtTest/QtTest>
#include <QTimer>

#if QT_VERSION >= 0x050000
#include <QtConcurrent/QtConcurrentRun>
#else
#include <QtConcurrentRun>
#endif

#include "test_serverinterface.h"
#include "test_mediadataserver.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Test_ServerInterface test_serverInterface;
    QTest::qExec(&test_serverInterface);

    Test_MediaDataServer test_mediaDataServer;
    QTest::qExec(&test_mediaDataServer);

    QTimer::singleShot(0, &app, SLOT(quit()));
    return app.exec();
//    app.exec();

//    return 0;
}
