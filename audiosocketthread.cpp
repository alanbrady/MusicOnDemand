#include "audiosocketthread.h"
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QHostAddress>
#include <QFile>

AudioSocketThread::AudioSocketThread(const int socketDesc, QObject *parent) :
    QThread(parent), m_sock(socketDesc)
{
    socket = new QTcpSocket(this);
    QObject::moveToThread(this);
    this->start();
}

void AudioSocketThread::run() {
    if (!socket->setSocketDescriptor(m_sock, QAbstractSocket::ConnectedState, QIODevice::ReadWrite)) {
        emit (error(socket->error()));
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
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

void AudioSocketThread::slotReadyRead() {
    QByteArray data;
    const qint16 bufSize = 16384;
    char buf[bufSize];
    QDataStream sockStrm(socket);
    sockStrm.setVersion(QDataStream::Qt_4_0);
    quint16 msgSize = 0;
    sockStrm >> msgSize;

    qint16 bytesRead = 0;
    qint16 totalBytesRead = 0;
    int bytesLeft = 0;
    qint16 maxRead = (msgSize < bufSize) ? msgSize : bufSize;

    QDataStream dataStrm(&data, QIODevice::ReadWrite);
    dataStrm.setVersion(QDataStream::Qt_4_0);

    do {
        bytesRead = socket->read(buf, maxRead);
        if (bytesRead == -1)
            return;
        totalBytesRead += bytesRead;
        dataStrm.writeRawData(buf, bytesRead);
        bytesLeft = msgSize - totalBytesRead;
        if (bytesLeft < maxRead)
            maxRead = bytesLeft;

    } while(bytesLeft > 0);
    this->sendData(this->readFile(data));
}

void AudioSocketThread::sendData(QByteArray dataToSend) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out.writeRawData(dataToSend, dataToSend.size());
    out.device()->seek(0);
    out << (quint16)(data.size() - sizeof(quint16));
    if (socket->write(data) == -1)
        emit error(socket->error());
    socket->disconnectFromHost();
    if (socket->state() == QAbstractSocket::ConnectedState)
        socket->waitForDisconnected();
    socket->close();
    this->quit();
}
