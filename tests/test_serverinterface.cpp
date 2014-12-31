#include <QtTest/QtTest>
#include <QTcpSocket>
#include <QSignalSpy>
#include <QDebug>

#include <serverinterface.h>

class MockServerInterface;

class Test_ServerInterface : public QObject
{
    Q_OBJECT
public:
    Test_ServerInterface() {
    }

    ~Test_ServerInterface() {}

private slots:
    void initTestCase();
    void test_parse_message();
    void test_incomplete_message();
    void test_message_too_large();

private:
};

class MockServerInterface : public ServerInterface
{
    Q_OBJECT
public:
    MockServerInterface(const quint16 port)
        : ServerInterface(port) {}
    void parseMessage(const char *msg, const unsigned int len, QTcpSocket *) {
        m_rcvd = QString::fromLocal8Bit(msg, len);
    }

    QString getReceivedMessage() const { return m_rcvd; }
private:
    QString m_rcvd;
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

QTEST_MAIN(Test_ServerInterface)
#include "test_serverinterface.moc"
