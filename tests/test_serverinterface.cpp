#include <QtTest/QtTest>
#include <QTcpSocket>

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
    void test_parseMessage();

private:
};

class MockServerInterface : public ServerInterface
{
    Q_OBJECT
public:
    MockServerInterface(const quint16 port)
        : ServerInterface(port) {}
    void parseMessage(const char *msg, const unsigned int len, QTcpSocket *) {
        m_rcvd = msg;
        m_rcvd += " Length: " + len;
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

void Test_ServerInterface::test_parseMessage()
{
    MockServerInterface server(49999);
    MockClient client;
    QVERIFY(client.state() == QAbstractSocket::ConnectedState);
    client.write("test");
    QCOMPARE(server.getReceivedMessage(), QString("test Length: 4"));
}

QTEST_MAIN(Test_ServerInterface)
#include "test_serverinterface.moc"
