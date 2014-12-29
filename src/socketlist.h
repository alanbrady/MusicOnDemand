#ifndef SOCKETLIST_H
#define SOCKETLIST_H

#include <QTcpSocket>
#include <QObject>

/* Description: SocketList provides a linked list especially made for QTcpSockets.
 *  The main goal is to provide a list which will clean itself up when a socket
 *  is disconnected.  The alternatives would be to either let the socket refer-
 *  ences sit in memory and eventually eat up all the memory or manually search
 *  for the socket which was disconnected which would take O(n) time.  Implment-
 *  ing a self-modifying list provides a way to cleanup disconnected sockets in
 *  O(1) time.
 *
 *  The reason for keeping a list of connected sockets in the first place insures
 *  resources get properly cleaned up when the program closes.
 */

class SocketListNode;

class SocketList
{
public:
    SocketList();
    ~SocketList();

    bool isEmpty() const;
    void push(QTcpSocket* socket);
    QTcpSocket* pop();

private:
    SocketListNode* m_head;
};

class SocketListNode : public QObject
{
    Q_OBJECT
public:
    SocketListNode(SocketListNode* next = 0,
                   QTcpSocket* socket = 0);
    ~SocketListNode();

    QTcpSocket* getSocket() const { return m_socket; }

    SocketListNode* next() const { return m_next; }
    void setNext(SocketListNode* node) { m_next = node; }
private:
    QTcpSocket* m_socket;
    SocketListNode* m_next;

private slots:
    void socketDisconnected();
};

#endif // SOCKETLIST_H
