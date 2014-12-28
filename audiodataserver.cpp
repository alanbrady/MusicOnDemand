#include "audiodataserver.h"

#include <QEventLoop>
#include <QDebug>

AudioDataServer::AudioDataServer(quint16 port)
    : ServerInterface(port)
{
}

void AudioDataServer::parseMessage(const char *msg, const unsigned int msgLen,
                                   QTcpSocket *sender)
{
    QString id = QString::fromLocal8Bit(msg, msgLen);
    QByteArray returnData = readFile(getFilePathForID(id));
    sendData(sender, returnData, returnData.length());

}

QByteArray AudioDataServer::readFile(const QString &path)
{
    QFile file(path);
    if (file.exists()) {
        QByteArray data;
        QDataStream ds(&data, QIODevice::WriteOnly);
        ds << file.readAll();
        return data;
    } else return 0;
}

QString AudioDataServer::getFilePathForID(const QString &id)
{
    return musicLibrary.getFilePathForId(id);
}


