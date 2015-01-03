#include <QSignalSpy>
#include <QDebug>

#include "test_serverinterface.h"
#include "mockclient.h"

void Test_ServerInterface::initTestCase()
{

}

void Test_ServerInterface::test_parse_message()
{
    MockServerInterface server(49999);
    MockClient client(49999);
    QVERIFY(client.waitForConnected(3000));
    char msg[7] = "00test";
    quint16 size = 4;
    memcpy(msg, &size, 2);
    client.write(msg, 6);
    QVERIFY(client.waitForBytesWritten(3000));
    QTest::qWait(500);
    QCOMPARE(server.getReceivedMessage(), QString("test"));
}

void Test_ServerInterface::test_incomplete_message()
{
    MockServerInterface server(50004);
    QSignalSpy serverSpy(&server, SIGNAL(error(QString)));
    MockClient client(50004);
    QVERIFY(client.waitForConnected(3000));
    char msg[7] = "--test";
    quint16 size = 20;
    memcpy(msg, &size, 2);
    client.write(msg,6);
    QVERIFY(client.waitForBytesWritten(3000));
    QTest::qWait(500);
    client.disconnectFromHost();
    QTest::qWait(500);
    QCOMPARE(serverSpy.count(), 1);
}

void Test_ServerInterface::test_message_too_large()
{
    MockServerInterface server(50001);
    QSignalSpy serverSpy(&server, SIGNAL(error(QString)));

    MockClient client(50001);
    QVERIFY(client.waitForConnected(3000));

    char msg[7] = "--test";
    quint16 size = 1025;
    memcpy(msg, &size, 2);
    client.write(msg, 7);
    QVERIFY(client.waitForBytesWritten(3000));
    QTest::qWait(500);
    QCOMPARE(serverSpy.count(), 1);
}

void Test_ServerInterface::test_server_send_message()
{
    char sendData[5] = "test";
    MockServerInterface server(50005, sendData, 4);
    MockClient client(50005);
    QVERIFY(client.waitForConnected(3000));

    char msg[7] = "--test";
    quint16 size = 4;
    memcpy(msg, &size, 2);
    client.write(msg, 7);
    QVERIFY(client.waitForBytesWritten(3000));
    QTest::qWait(500);

    char recvData[6];
    char recvDataCmp[7] = "--test";
    memcpy(recvDataCmp, &size, 2);
//    QCOMPARE((int)client.bytesAvailable(), 6);
    QCOMPARE((int)client.read(recvData, 6), 6);
    QVERIFY(memcmp(recvData, recvDataCmp, 6) == 0);
}
