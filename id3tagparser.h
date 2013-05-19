#ifndef ID3TAGPARSER_H
#define ID3TAGPARSER_H

#include <QString>
#include <QHash>
#include <QFile>
#include <QDataStream>

#include "tag.h"

class ID3TagParser
{
public:
    enum Mode {
        V1,
        V2
    };

    ID3TagParser(Mode mode, bool getExtendedTags = false);

//    bool getTag(const QString& fileName, Tag* tag, bool getExt) const;
    Tag getTag(const QString& fileName) const;
private:
    const Mode m_mode;
    const bool m_getExt;

    void getTagID3v1(QFile& file, Tag* tag) const;
    void getTagID3v2(QFile& file, Tag* tag) const;

    QString parseFrameData(char* buf, const quint8 size) const;
    unsigned int getFrameDataSize(char* buf, char id3ver) const;
    unsigned char parseTrack(const QString& str) const;

};

#endif // ID3TAGPARSER_H
