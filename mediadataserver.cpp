#include "mediadataserver.h"
#include <QFile>
#include <QDir>
#include <QFIleInfo>
#include <QTemporaryFile>
#include <QImage>
#include <QDateTime>
#include <QBuffer>
#include "musiclibrary.h"

#include <QDebug>

MediaDataServer::MediaDataServer(QObject *parent)
    : QTcpServer(parent), parser(ID3TagParser::V2, false)
{
}

void MediaDataServer::incomingConnection(int socketDesc) {
    MediaSocketThread *thread = new MediaSocketThread(socketDesc);
    connect (thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect (thread, SIGNAL(messageReceived(MediaSocketThread*, QByteArray)),
             this, SLOT(receiveMessage(MediaSocketThread*, QByteArray)));
}

void MediaDataServer::receiveMessage(MediaSocketThread *thread, QByteArray msgData) {
    QDataStream in(&msgData, QIODevice::ReadOnly);
    quint16 intm;
    in >> intm;
    MESSAGE m = MESSAGE(intm);
    uint msgSize = msgData.size() - sizeof(quint16);
    char* msgStr = new char[msgSize];
    in >> msgStr;
    QByteArray data = getData(m, msgStr);
    if (!data.isEmpty())
        QMetaObject::invokeMethod(thread, "sendData", Qt::QueuedConnection, Q_ARG(QByteArray, data));
    delete msgStr;
}

QByteArray MediaDataServer::getData(MESSAGE m, QString msg) const {
    switch(m) {
    case LIB_MOD_DATE_REQ:
        return getLibraryLastModified();
    case LIB_FILE_REQ:
        return getLibraryDatabase();
    case ALBUM_ART_REQ:
        return getAlbumArt(msg);
    case ID3_REQ:
        return getID3Tags(msg);
    default:
        return 0;
    }
}

QByteArray MediaDataServer::getLibraryLastModified() const {
    QFileInfo libFile(MusicLibrary::LIBRARYPATH);
    if (libFile.exists()) {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_0);
        out << libFile.lastModified().toMSecsSinceEpoch();
        return data;
    } else return 0;
}

QByteArray MediaDataServer::getLibraryDatabase() const {
    QFile libFile(MusicLibrary::LIBRARYPATH);
    if (libFile.exists()) {
        return libFile.read(libFile.size());
    } else return 0;
}

QByteArray MediaDataServer::getAlbumArt(const QString &filePath) const {
    QDir fileDir = QFileInfo(filePath).dir();
    QFile lucky(fileDir.path() + "/Folder.jpg");
    QImage img;
    if (lucky.exists())
        img.load(lucky.fileName());
    else {
        QStringList nameFilters;
        nameFilters << "*.jpg" << "*.png";
        fileDir.setNameFilters(nameFilters);
        fileDir.setFilter(QDir::Files);
        QStringList files = fileDir.entryList();
        if (!files.isEmpty()) {
            QString newPath = fileDir.path() + "/" + files.at(0);
            if (!img.load(newPath))
                return 0;
        } else return 0;
    }
    img = img.scaledToWidth(200);
    QByteArray imgData;
    QBuffer out(&imgData);
    out.open(QIODevice::WriteOnly);
    img.save(&out, "JPG", 30);
    return imgData;
}

QByteArray MediaDataServer::getID3Tags(const QString &filePath) const {
    Tag tag = parser.getTag(filePath);
    if (tag.isValid())
        return tagToBytes(tag);
    else
        return "NULL";
}

QByteArray MediaDataServer::tagToBytes(Tag tag) const {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    QString tagStr;

    tagStr += "Artist | " + tag.getArtist() + "\n";
    tagStr += "Album | " + tag.getAlbum() + "\n";
    tagStr += "Title | " + tag.getTitle() + "\n";
    tagStr += "Track | " + tag.getTrack();

    out.writeRawData(tagStr.toUtf8(), tagStr.size());
    return data;
}
