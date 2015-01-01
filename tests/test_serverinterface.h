#ifndef TEST_SERVERINTERFACE
#define TEST_SERVERINTERFACE

#include <QtTest/QtTest>
#include <serverinterface.h>

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

#endif // TEST_SERVERINTERFACE

