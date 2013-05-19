#include "id3tagparser.h"

#include <QDebug>

ID3TagParser::ID3TagParser(Mode mode, bool getExtendedTags)
    : m_mode(mode), m_getExt(getExtendedTags)
{
}

//bool ID3TagParser::getTag(const QString &fileName, Tag *tag, bool getExt) const
Tag ID3TagParser::getTag(const QString &fileName) const
{
    Tag tag;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        if (m_mode == V1) {
            getTagID3v1(file, &tag);
        } else if (m_mode == V2) {
            getTagID3v2(file, &tag);
        }
        file.close();
    }
    return tag;
}

void ID3TagParser::getTagID3v1(QFile &file, Tag *tag) const
{
    int bufLen = 31;
    char* buf;
    QDataStream data(&file);

    if (!file.seek(file.size()-128)) {
        qDebug() << "Error: unable to parse tag";
        return;
    }

    buf = new char[bufLen]();
    buf[30] = '\0';

    data.readRawData(buf, 3);
    buf[3] = '\0';
    if (strcmp(buf, "TAG") == 0) {
        memset(buf, 0, 3);

        data.readRawData(buf, 30);
        tag->setTitle(QString::fromLocal8Bit(buf));
        memset(buf, 0, 30);

        data.readRawData(buf, 30);
        tag->setArtist(QString::fromLocal8Bit(buf));
        memset(buf, 0, 30);

        data.readRawData(buf, 30);
        tag->setAlbum(QString::fromLocal8Bit(buf));
        memset(buf, 0, 30);

        data.readRawData(buf, 4);
        buf[4] = '\0';
        tag->setYear(atoi(buf));
        memset(buf, 0, 4);

        file.seek(file.pos()+28);
        data.readRawData(buf, 2);
        if (buf[0] == '\0') {
            tag->setTrack(buf[1]);
        }
        memset(buf, 0, 2);

        data.readRawData(buf, 1);
        tag->setGenre(buf[0]);

    }
    delete buf;
}

void ID3TagParser::getTagID3v2(QFile &file, Tag *tag) const
{
    QDataStream dataStream(&file);
    const unsigned int initBufSize = 10;
    char* buf;
    char id3ver = 0;
    quint16 tagSize = 0;
    bool unsynched = false;
    bool extHeader = false;

    buf = new char[initBufSize]();

    if (dataStream.readRawData(buf, 10) < 10) {
        delete buf;
        return;
    }

    // make sure there's an ID3v2 tag else, default to ID3v1
    if (!(strncmp(buf, "ID3", 3) == 0)) {
        delete buf;
//        qDebug() << "Error: no ID3v2 tag, reverting to v1";
        getTagID3v1(file, tag);
        return;
    }

    id3ver = buf[3];

    // if version isn't right, then default to ID3v1
    if (id3ver < 0 || id3ver > 4) {
        delete buf;
        getTagID3v1(file, tag);
        return;
    }

    tagSize = (((buf[9] & 0xFF) | ((buf[8] & 0xFF) << 7) | ((buf[7]) & 0xFF) << 14)|
            ((buf[6] & 0xFF) << 21));

    unsynched = (buf[5] & 0x80) != 0 ? true:false;
    extHeader = (buf[5] & 0x40) != 0 ? true:false;

    memset(buf, 0, initBufSize);
    if (extHeader) {
        if (dataStream.readRawData(buf, 4) > 0) {
            int headerSize = (buf[3] << 21) | (buf[2] << 14) | (buf[1] << 7) | buf[0];
            dataStream.skipRawData(headerSize - 4);
            tagSize -= headerSize;
        }
    }

    delete buf;
    buf = new char[tagSize];
    if (dataStream.readRawData(buf, tagSize) < 0) {
        delete buf;
        return;
    }

    if (unsynched) {
        int newPos = 0;
        char* temp = new char[tagSize];

        // unsynching add a 0x00 after every 0xFF to prevent MP3 synching
        // this code removes the extra 0x00
        for (int i = 0; i < tagSize; i++) {
            if (i < tagSize-1 && (buf[i] & 0xFF) == 0xFF && buf[i+1] == 0) {
                temp[newPos++] = 0xFF;
                i++;
            } else
                temp[newPos++] = buf[i];

        }

        tagSize = newPos;
        delete buf;
        buf = temp;
    }

    char frameHeaderSize = 0;
    if (id3ver < 3)
        frameHeaderSize = 6;
    else
        frameHeaderSize = 10;


    char frameNameSize;
    if (id3ver < 3)
        frameNameSize = 3;
    else
        frameNameSize = 4;

    char* frameName = new char[frameNameSize]();

    for (int pos = 0; pos < tagSize;) {
        memcpy(frameName, buf+pos, frameNameSize);
        const quint8 frameSize = getFrameDataSize(buf+(pos+frameNameSize),
                                                  id3ver);

        char* dataPos = buf+frameHeaderSize+pos;

        if (id3ver < 3) {
            if (strncmp(frameName, "TT2", 3) == 0) {
                tag->setTitle(parseFrameData(dataPos, frameSize));
            } else if (strncmp(frameName, "TAL", 3) == 0) {
                tag->setAlbum(parseFrameData(dataPos, frameSize));
            } else if (strncmp(frameName, "TP1", 3) == 0) {
                tag->setArtist(parseFrameData(dataPos, frameSize));
            } else if (strncmp(frameName, "TP2", 3) == 0) {
                tag->setAlbumArtist(parseFrameData(dataPos, frameSize));
            } else if (strncmp(frameName, "TRK", 3) == 0) {
                tag->setTrack(parseTrack(parseFrameData(dataPos, frameSize)));
            } else if (strncmp(frameName, "TYE", 3) == 0) {
                tag->setYear(parseFrameData(dataPos, frameSize).toInt());
            } else if (strncmp(frameName, "TCO", 3) == 0) {
                tag->setGenre(parseFrameData(dataPos, frameSize));
            } else if (m_getExt) {
                tag->addExtendedData(frameName,
                                     parseFrameData(dataPos, frameSize));
            }
        } else {
            if (strncmp(frameName, "TIT2", 4) == 0) {
                tag->setTitle(parseFrameData(dataPos, frameSize));
            } else if (strncmp(frameName, "TALB", 4) == 0) {
                tag->setAlbum(parseFrameData(dataPos, frameSize));
            } else if (strncmp(frameName, "TPE1", 4) == 0) {
                tag->setArtist(parseFrameData(dataPos, frameSize));
            } else if (strncmp(frameName, "TPE2", 4) == 0) {
                tag->setAlbumArtist(parseFrameData(dataPos, frameSize));
            } else if (strncmp(frameName, "TRCK", 4) == 0) {
                tag->setTrack(parseTrack(parseFrameData(dataPos, frameSize)));
            } else if (strncmp(frameName, "TYER", 4) == 0) {
                tag->setYear(parseFrameData(dataPos, frameSize).toInt());
            } else if (strncmp(frameName, "TCON", 4) == 0) {
                tag->setGenre(parseFrameData(dataPos, frameSize));
            } else if (m_getExt) {
                tag->addExtendedData(frameName,
                                     parseFrameData(dataPos, frameSize));
            }
        }
        pos += frameHeaderSize+frameSize;
    }
}

QString ID3TagParser::parseFrameData(char *buf, const quint8 size) const
{
    QString str;
    char textEncoding = buf[0] & 0xFF;
    if (textEncoding == 1) {
        str = QString::fromUtf16((ushort*)(buf+1), size-1);
    } else if (textEncoding == 0){
        str = QString::fromLocal8Bit(buf+1, size-1);
    } else if (textEncoding == 3) {
        // haven't figured out what text encoding 3 might be but it has an
        // extra byte on the end for some reason
        str = QString::fromLocal8Bit(buf+1, size-2);
    }
    return str;
}

quint8 ID3TagParser::getFrameDataSize(char *buf, char id3ver) const
{
    quint8 size;

    // size integer must be unsynched

    if (id3ver < 3) {
        size = (buf[2] & 0xFF) | ((buf[1] & 0xFF) << 8) |
                ((buf[0] & 0xFF) << 16);
    }
    else if (id3ver < 4) {
        size = (buf[3] & 0xFF) | ((buf[2] & 0xFF) << 8) |
                ((buf[1] & 0xFF) << 16) | ((buf[0] & 0xFF) << 24);
    } else {
        size = (buf[3] & 0xFF) | ((buf[2] & 0xFF) << 7) |
                ((buf[1] & 0xFF) << 14) | ((buf[0] & 0xFF) << 21);
    }

    return size;
}

unsigned char ID3TagParser::parseTrack(const QString &str) const
{
    int slash = str.indexOf("/");
    if (slash != -1)
        return str.left(slash).toInt();
    else
        return str.toInt();
}




