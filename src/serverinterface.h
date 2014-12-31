#ifndef SERVERINTERFACE_H
#define SERVERINTERFACE_H

#include <QTcpServer>
#include <QTcpSocket>

#if QT_VERSION >= 0x050000
#include <QtConcurrent/QtConcurrentRun>
#else
#include <QtConcurrentRun>
#endif


#include "socketlist.h"

#define MAX_READ 1024

/* Description: ServerInterface provides a base class in which additional server
 *  implementations can inherit from and provides a unified methodology for
 *  sending and receiving information via QTcpServer.  It also abstracts away
 *  the connection handling code and threading.
 */


class ServerInterface : public QObject
{
    Q_OBJECT
public:
    ServerInterface(quint16 port);
    virtual ~ServerInterface();


signals:
    void error(QString msg);

protected:
    void sendData(QTcpSocket* socket, const char* data, const quint16 dataLen);
    virtual void parseMessage(const char*, const unsigned int, QTcpSocket*) = 0;

private:
    const quint16 m_port;
    QTcpServer m_server;
    SocketList m_connections;

    void doSocketRead(QTcpSocket* socket);

private slots:
    void socketReadyRead();
    void clientAwaitingConnect();
};

#endif // SERVERINTERFACE_H
