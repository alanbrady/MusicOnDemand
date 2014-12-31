#include "serverinterface.h"

ServerInterface::ServerInterface(quint16 port)
    : m_port(port)
{
    connect(&m_server, SIGNAL(newConnection()),
            this, SLOT(clientAwaitingConnect()));

    m_server.listen(QHostAddress::Any, m_port);
}
ServerInterface::~ServerInterface()
{
    m_server.close();
}



void ServerInterface::sendData(QTcpSocket *socket, const char *data,
                               const quint16 dataLen)
{
    char dataLenMsg[2];
    // pack the data length into a two 8 bit chars since dataLen is 16 bits
    dataLenMsg[0] = (char)dataLen;
    dataLenMsg[1] = (char)(dataLen >> 8);
    socket->write(dataLenMsg, 2);
    socket->write(data, dataLen);
}

void ServerInterface::doSocketRead(QTcpSocket *socket)
{
    while (socket->bytesAvailable() > 0) {
        // read in message size and put it into a 16 bit unsigned integer
        quint16 msgSize;
        char msgSizeBuf[2];
        socket->read(msgSizeBuf, 2);
        memcpy(&msgSize, msgSizeBuf, 2);

        // limit the read size to prevent malicious read requests
        if (msgSize > MAX_READ) {
            emit error("Error: read size too large.");
        } else {
            // until we read the entire message, read socket into data array
            char* data = new char[msgSize];
            qint16 bytesRead = 0;
            quint16 totalBytesRead = 0;
            quint16 bytesLeft = msgSize;
            do {
                bytesRead = socket->read(data+totalBytesRead, bytesLeft);
                if (bytesRead != -1) {
                    totalBytesRead += bytesRead;
                    bytesLeft -= bytesRead;
                } else {
                    // socket read returned -1, either disconnect or not all
                    // data was received
                    break;
                }
            } while (bytesLeft > 0);

            if (bytesLeft == 0) {
                parseMessage(data, msgSize, socket);
            } else {
                emit error("Error: incomplete socket message received.");
            }
            delete[] data;
        }
    }
}

void ServerInterface::socketReadyRead()
{
    // retrieve the socket that issued ready read and parse data in a separate
    // thread object (QtConcurrent::run)
    QTcpSocket* socket = static_cast<QTcpSocket*>(QObject::sender());
    QtConcurrent::run(this, &ServerInterface::doSocketRead, socket);
}

void ServerInterface::clientAwaitingConnect()
{
    // get pending connection, connect the readyRead signal and save reference
    // in our SocketList data structure
    QTcpSocket* connection = 0;
    while ((connection = m_server.nextPendingConnection()) != 0) {
        connect(connection, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
        m_connections.push(connection);
    }

}
