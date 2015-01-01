#ifndef MOCKCLIENT
#define MOCKCLIENT

#include <QTcpSocket>
#include <QHostAddress>

class MockClient : public QTcpSocket {
    Q_OBJECT
public:
    MockClient(quint16 port) {
        connectToHost(QHostAddress::LocalHost, port);
    }

    ~MockClient() {

    }
};

#endif // MOCKCLIENT

