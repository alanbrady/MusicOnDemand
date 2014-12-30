#include "socketlist.h"

SocketList::SocketList()
    : m_head(new SocketListNode())
{

}

SocketList::~SocketList()
{
    // m_head should never be null, instead m_head can hold an invalid object
    SocketListNode* iter = m_head;
    SocketListNode* temp;
    do {
        temp = iter->next();
        delete iter;
        iter = temp;
    } while (temp != 0);
}

bool SocketList::isEmpty() const
{
    // list is empty when head's socket is invalid
    return m_head->getSocket() == 0;
}

void SocketList::push(QTcpSocket *socket)
{
    SocketListNode* newNode = new SocketListNode(m_head, socket);
    m_head = newNode;
}

QTcpSocket *SocketList::pop()
{
    QTcpSocket* ret = m_head->getSocket();
    // don't delete memory if attempting to pop the last element
    // last element has an invalid socket pointer to signal end of list
    if (ret != 0) {
        SocketListNode* temp = m_head->next();
        delete m_head;
        m_head = temp;
    }
    return ret;
}



SocketListNode::SocketListNode(SocketListNode *next,
                               QTcpSocket *socket)
    : m_socket(socket), m_next(next)
{
    // if socket disconnects, we want to remove from list to avoid eating up
    // all the memory
    if (socket != 0) {
        connect(m_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    }
}

SocketListNode::~SocketListNode()
{
    // cleanup resources inline with RAII
    if (m_socket != 0) {
        if (m_socket->isOpen()) {
            m_socket->close();
        }
        delete m_socket;
    }
}

void SocketListNode::socketDisconnected()
{
    // if socket disconnects, need to cleanup memory and remove self from list
    // since the node only knows about itself and next, we copy next into self
    // and delete next
    SocketListNode* temp = m_next;
    delete m_socket;
    m_socket = m_next->getSocket();
    m_next = m_next->next();
    delete temp;
}
