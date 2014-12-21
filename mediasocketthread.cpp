#include "mediasocketthread.h"

MediaSocketThread::MediaSocketThread(int socketDesc)
    : QThread(), m_sock(socketDesc)
{
    m_socket = new QTcpSocket(this);
    QObject::moveToThread(this);
    this->start();
}

MediaSocketThread::~MediaSocketThread() {
    this->quit();
    this->wait();

    m_socket->disconnectFromHost();
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->waitForDisconnected();
    m_socket->close();
    delete m_socket;
}

void MediaSocketThread::run() {
    if (!m_socket->setSocketDescriptor(m_sock)) {
        emit error(m_socket->error());
        return;
    }

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    this->exec();
}

void MediaSocketThread::slotReadyRead() {
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

//    emit messageReceived(this, data);
    char msgSizeBuf[2];
    m_socket->read(msgSizeBuf, 2);
    unsigned int msgSize = 0;
    memcpy(&msgSize, msgSizeBuf, 2);

    char* buf = new char[msgSize];

    int totalBytesRead = 0;
    int bytesLeft = msgSize;
    do {
        int bytesRead = m_socket->read(buf+totalBytesRead, msgSize);
        if (bytesRead == -1) {
            emit(error(m_socket->error()));
        } else {
            totalBytesRead += bytesRead;
            bytesLeft -= bytesRead;
        }
    } while(bytesLeft > 0);

    QByteArray data(buf, msgSize);
    emit messageReceived(this, data);
}

void MediaSocketThread::sendData(QByteArray dataToSend) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out.writeRawData(dataToSend, dataToSend.size());
    out.device()->seek(0);
    out << (quint16)(data.size() - sizeof(quint16));
    if (m_socket->write(data) == -1)
        emit error(m_socket->error());
}
