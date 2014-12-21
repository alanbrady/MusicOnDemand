#include "audiosocketthread.h"
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QHostAddress>
#include <QFile>

AudioSocketThread::AudioSocketThread(const int socketDesc, QObject *parent) :
    QThread(parent), m_sock(socketDesc), musicLibrary(this, "audio_socket_library")
{
    m_socket = new QTcpSocket(this);
    QObject::moveToThread(this);
    this->start();
}

void AudioSocketThread::run() {
    if (!m_socket->setSocketDescriptor(m_sock, QAbstractSocket::ConnectedState, QIODevice::ReadWrite)) {
        emit (error(m_socket->error()));
        return;
    }

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    this->exec();

//    QTcpSocket tcpSocket;
//    if (!tcpSocket.setSocketDescriptor(socketDescriptor,
//                                       QAbstractSocket::ConnectedState,
//                                       QIODevice::WriteOnly)) {
//        emit error(tcpSocket.error());
//        return;
//    }



//    delete data;

//    out.device()->seek(0);
//    out << (quint16)(block.size() - sizeof(quint16));

//    int sizeOfFile = file.size();
//    int bytesWritten = 0;
//    int totalBytes = 0;
//    int maxBytes = 2056;
//    while (totalBytes < sizeOfFile &&
//           (bytesWritten = tcpSocket.write(block.data(), maxBytes)) != -1) {
//        totalBytes += bytesWritten;
//        tcpSocket.waitForBytesWritten();
//        block.remove(0, bytesWritten);
//    }
//    while ((bytesWritten += tcpSocket.write(block)) < sizeOfFile) {
//        tcpSocket.waitForBytesWritten();
//    };

//    tcpSocket.disconnectFromHost();
//    if (tcpSocket.state() == QAbstractSocket::ConnectedState)
//        tcpSocket.waitForDisconnected();
//    tcpSocket.close();
}


QByteArray AudioSocketThread::readFile(const QString& path) {
    QFile file(path);
    if (file.exists()) {
        QByteArray data;
        QDataStream ds(&data, QIODevice::WriteOnly);
        ds << file.readAll();
        return data;
    } else return 0;
}

QString AudioSocketThread::getFilePathForID(const QString &id)
{
    return musicLibrary.getFilePathForId(id);
}

void AudioSocketThread::slotReadyRead() {
    char msgSizeBuf[2];
    m_socket->read(msgSizeBuf, 2);  // read in message size
    // TODO: needs a safety net to make sure a message that's too large isn't
    // attempted

//    unsigned int msgSize = (int)msgSizeBuf;
    unsigned int msgSize = 0;
    memcpy(&msgSize, msgSizeBuf, 2);

    char* buf = new char[msgSize+1];
    buf[msgSize] = '\0'; // put in null terminator
    int bytesLeft = msgSize;
    int totalBytesRead = 0;

    // keep reading on the socket until the entire message is received
    do {
        int bytesRead = m_socket->read(buf+totalBytesRead, bytesLeft);
        if (bytesRead == -1) {
            emit error(m_socket->error());
        } else {
            bytesLeft -= bytesRead;
            totalBytesRead += bytesRead;
        }
    } while (bytesLeft > 0);

    // translate the message (sql db id) into a file path
    QString id(buf);
    QString filePath = getFilePathForID(id);

    // read in file data and send it
    sendData(readFile(filePath));
    delete buf;
}

void AudioSocketThread::sendData(QByteArray dataToSend) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out.writeRawData(dataToSend, dataToSend.size());
    out.device()->seek(0);
    out << (quint16)(data.size() - sizeof(quint16));
    if (m_socket->write(data) == -1)
        emit error(m_socket->error());

    // I don't recall why I thought it necessary to close the socket
    // after sending a single file.  Probably a mistake
    m_socket->disconnectFromHost();
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->waitForDisconnected();
    m_socket->close();
    this->quit();
}
