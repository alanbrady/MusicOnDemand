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

MediaDataServer::MediaDataServer(quint16 port)
    : ServerInterface(port), m_id3Parser(ID3TagParser::V2, false)
{
}

void MediaDataServer::parseMessage(const char *data, const unsigned int dataLen,
                                   QTcpSocket *sender)
{
    Message msg;
    memcpy(&msg, data, 2);
    QString requestString = QString::fromLocal8Bit(data+2, dataLen-2);
    QByteArray returnData = getReturnData(msg, requestString);
    sendData(sender, returnData, returnData.length());
}

QByteArray MediaDataServer::getReturnData(Message msg,
                                          const QString& requestData) const {
    switch(msg) {
    case LIB_MOD_DATE_REQ:
        return getLibraryLastModified();
    case LIB_FILE_REQ:
        return getLibraryDatabase();
    case ALBUM_ART_REQ:
        return getAlbumArt(requestData);
    case ID3_REQ:
        return getID3Tags(requestData);
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
    Tag tag = m_id3Parser.getTag(filePath);
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
