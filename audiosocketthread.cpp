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


//    QFile file(path);
//    QByteArray block;
//    if (!file.exists())
//        return block;
//    file.open(QIODevice::ReadOnly);
//    QDataStream out(&block, QIODevice::WriteOnly);
//    QDataStream in(&file);
//    char* data = new char[16384];
//    int bytesRead = 0;

//    out.setVersion(QDataStream::Qt_4_0);
//    in.setVersion(QDataStream::Qt_4_0);

//    out << (quint16)0;

//    while ((bytesRead = in.readRawData(data, sizeof(data))) > 0) {
//        out.writeRawData(data, bytesRead);
//    }
//    QByteArray dataArray(data);
//    delete data;
    //    return dataArray;
}

QString AudioSocketThread::getFilePathForID(const QString &id)
{
    return musicLibrary.getFilePathForId(id);
}

void AudioSocketThread::slotReadyRead() {
//    QByteArray data;
//    const qint16 bufSize = 16384;
//    char buf[bufSize];
//    QDataStream sockStrm(socket);
//    sockStrm.setVersion(QDataStream::Qt_4_0);
//    quint16 msgSize = 0;
//    sockStrm >> msgSize;

//    qint16 bytesRead = 0;
//    qint16 totalBytesRead = 0;
//    int bytesLeft = 0;
//    qint16 maxRead = (msgSize < bufSize) ? msgSize : bufSize;

//    QDataStream dataStrm(&data, QIODevice::ReadWrite);
//    dataStrm.setVersion(QDataStream::Qt_4_0);

//    do {
//        bytesRead = socket->read(buf, maxRead);
//        if (bytesRead == -1)
//            return;
//        totalBytesRead += bytesRead;
//        dataStrm.writeRawData(buf, bytesRead);
//        bytesLeft = msgSize - totalBytesRead;
//        if (bytesLeft < maxRead)
//            maxRead = bytesLeft;

//    } while(bytesLeft > 0);
//    this->sendData(this->readFile(data));

    char msgSizeBuf[2];
    m_socket->read(msgSizeBuf, 2);  // read in message size
    // TODO: needs a safety net to make sure a message that's too large isn't
    // attempted

    unsigned int msgSize = (int)msgSizeBuf;
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
