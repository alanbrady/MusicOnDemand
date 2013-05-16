//#include "id3taginterface.h"
//#include <QDebug>
////#include <QChar>
//#include <QFile>
////#include <fstream>
//#include <string>
//#include "tag.h"
//#include "id3tagparser.h"

//const int ID3TagInterface::ID3v1TagSize = 128;

//ID3TagInterface::QTags ID3TagInterface::getTags(QString file) {
////    TagLib::FileRef f(TagLib::FileName(file.constData()));
////    TagLib::FileRef f (QFile::encodeName(file).constData());
//    ID3TagParser parser(ID3TagParser::V2, false);
//    Tag tag;
//    parser.getTag(file, &tag, false);
//    if (tag.isValid()) {
//        QTags tags;
////        TagLib::Tag* t = f.tag();
////        tags.append(qMakePair(QString("Artist"), QString(t->artist().toCString())));
////        tags.append(qMakePair(QString("Album"), QString(t->album().toCString())));
////        tags.append(qMakePair(QString("Track"), QString(t->title().toCString())));
////        tags.append(qMakePair(QString("TrackNum"), QString().number(t->track())));
//        tags.append(qMakePair(QString("Artist"), tag.getArtist()));
//        tags.append(qMakePair(QString("Album"),tag.getAlbum()));
//        tags.append(qMakePair(QString("Track"), tag.getTitle()));
//        tags.append(qMakePair(QString("TrackNum"), QString().setNum(tag.getTrack())));
//        return tags;
//    }
//    return QTags();
//}

//ID3TagInterface::QTagMap ID3TagInterface::getTagMap(QString file) {
////    ID3TagInterface::QTagMap ID3TagInterface::getTagMap(const char* file) {
////    char suffix[4];
////    int len = strlen(file);
////    strncpy(suffix, file+(len-3), 3);
////    qDebug() << suffix;

////    if (strcmp(suffix, "mp3") == 0) {
////        std::ifstream mp3file;
////        ID3v1Tag id3tag;
////        char buf[ID3v1TagSize];
////        mp3file.open(file, std::ios_base::in | std::ios_base::binary);
////        if (!mp3file)
////            return map;
////        mp3file.seekg(-(ID3v1TagSize), std::ios::end);
////        if (mp3file.fail())
////            qDebug() << "Failed to seek file.";
//////        mp3file.read(reinterpret_cast<char*>(&id3tag), ID3v1TagSize);
////        mp3file.read(buf, ID3v1TagSize);
////        if (mp3file.fail())
////            qDebug() << "Filed to read file.";
////        qDebug() << buf;
////    } else {
////        QTagMap map;

////        TagLib::FileRef f(TagLib::FileName(file.constData()));
////        TagLib::FileRef f (QFile::encodeName(file).constData());
////        TagLib::Tag* t = 0;
////        if (!f.isNull() && (t = f.tag())) {

////            map.insert("Artist", t->artist().toCString());
////            map.insert("Album", t->album().toCString());
////            map.insert("Track", t->title().toCString());
////            map.insert("TrackNum", QString::number(t->track()));
////        } else qDebug() << "Unable to open file: " << QFile::encodeName(file).constData();
////    }
////    return map;
//    return QTagMap();
//}
