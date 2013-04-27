#include "audiodataserver.h"
#include "audiosocketthread.h"
//#include "test/mock_audiodataclient.h"

#include <QEventLoop>
#include <QDebug>

AudioDataServer::AudioDataServer(QObject *parent)
    : QTcpServer(parent)
{
}

void AudioDataServer::incomingConnection(int socketDesc) {
    AudioSocketThread *thread = new AudioSocketThread(socketDesc);
    connect (thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
//    thread->start();
}


//// Unit Test ************************************************

//#ifdef DEBUG

//void AudioDataServer_Test::test_serverConnection() {
//    AudioDataServer server;
//    server.setFilePath("simple.txt");
//    QVERIFY(server.listen(QHostAddress::LocalHost, 50008));
//    QVERIFY(server.isListening());
//    qDebug() << "awaiting client connection...";
//    QVERIFY(server.waitForNewConnection(-1));
//    QTest::qSleep(1000);
//}

//#endif // DEBUG
