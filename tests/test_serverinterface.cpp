#include <QtTest/QtTest>
#include <QTcpSocket>
#include <QSignalSpy>
#include <QDebug>

#include <serverinterface.h>

class MockServerInterface;

class Test_ServerInterface : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void test_parse_message();
    void test_incomplete_message();
    void test_message_too_large();
    void test_server_send_message();

};

class MockServerInterface : public ServerInterface
{
    Q_OBJECT
public:
    MockServerInterface(const quint16 port, const char*
                        sendData = 0, quint16 sendDataSize = 0)
        : ServerInterface(port), m_sendData(sendData),
          m_sendDataSize(sendDataSize) {}

    void parseMessage(const char *msg, const unsigned int len, QTcpSocket *socket) {
        m_rcvd = QString::fromLocal8Bit(msg, len);
        if (m_sendData != 0) {
            sendData(socket, m_sendData, m_sendDataSize);
        }
    }

    QString getReceivedMessage() const { return m_rcvd; }
private:
    QString m_rcvd;
    const char* m_sendData;
    quint16 m_sendDataSize;
};

class MockClient : public QTcpSocket {
    Q_OBJECT
public:
    MockClient() {
        connectToHost(QHostAddress::LocalHost, 49999);
    }

    ~MockClient() {

    }
};

void Test_ServerInterface::initTestCase()
{

}

void Test_ServerInterface::test_parse_message()
{
    MockServerInterface server(49999);
    MockClient client;
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
    MockServerInterface server(49999);
    QSignalSpy serverSpy(&server, SIGNAL(error(QString)));
    MockClient client;
    QVERIFY(client.waitForConnected(3000));
    char msg[7] = "--test";
    quint16 size = 20;
    memcpy(msg, &size, 2);
    client.write(msg,7);
    QVERIFY(client.waitForBytesWritten(3000));
    client.disconnectFromHost();
    QTest::qWait(500);
    QCOMPARE(serverSpy.count(), 1);
}

void Test_ServerInterface::test_message_too_large()
{
    MockServerInterface server(49999);
    QSignalSpy serverSpy(&server, SIGNAL(error(QString)));

    MockClient client;
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
    MockServerInterface server(49999, sendData, 4);
    MockClient client;
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
    QCOMPARE(client.read(recvData, 6), 6);
    QVERIFY(memcmp(recvData, recvDataCmp, 6) == 0);
}

QTEST_MAIN(Test_ServerInterface)
#include "test_serverinterface.moc"
