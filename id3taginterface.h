#ifndef ID3TAGINTERFACE_H
#define ID3TAGINTERFACE_H

#include <QList>
#include <QPair>
#include <QMap>
//#include <tag.h>
//#include <fileref.h>
//#include <mpegfile.h>
//#include <id3v2/id3v2frame.h>
//#include <id3v2/id3v2tag.h>
//#include <id3v2/id3v2header.h>

class ID3TagInterface {
public:    

    struct ID3v1Tag {
        char tag[3];
        char title[30];
        char artist[30];
        char album[30];
        char year[4];
        char comment[28];
        char track[2];
        char genre;
    };

    static const int ID3v1TagSize;

    typedef QPair<QString, QString> QTagPair;
    typedef QList<QTagPair> QTags;
    typedef QMap<QString, QString> QTagMap;

    static QTags getTags(QString file);
    static QTagMap getTagMap(QString file);

private:    
};

#endif // ID3TAGINTERFACE_H
