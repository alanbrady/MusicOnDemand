#ifndef TAG_H
#define TAG_H

#include <QString>
#include <QHash>

class Tag
{
public:
    Tag();
    ~Tag();

    Tag(const Tag& other);
    Tag& operator=(const Tag& other);

    bool isValid() const { return m_valid; }
    const QString& getTitle() const { return m_title; }
    const QString& getArtist() const { return m_artist; }
    const QString& getAlbumArtist() const { return m_albumArtist; }
    const QString& getAlbum() const { return m_album; }
    const quint16& getYear() const { return m_year; }
    unsigned char getTrack() const { return m_track; }
    const QString& getGenre() const { return m_genre; }
    const QHash<QString, QString>& getExtendedData() const
        { return (*m_ext); }

//    void setTitle(const char* title);
//    void setArtist(const char* artist);
//    void setAlbumArtist(const char* albumArtist);
//    void setAlbum(const char* album);
//    void setYear(const quint16 year);
//    void setTrack(const unsigned char track);
//    void setGenre(const unsigned char genre);
//    void addExtendedData(const QString& key, const QString& value);

    void setTitle(const QString& title);
    void setArtist(const QString& artist);
    void setAlbumArtist(const QString& albumArtist);
    void setAlbum(const QString& album);
    void setYear(const quint16 &year);
    void setTrack(const unsigned char track);
    void setGenre(const unsigned char genre);
    void setGenre(const QString& genre);
    void addExtendedData(const QString& key, const QString& value);

private:
    bool m_valid;
    QString m_title;
    QString m_artist;
    QString m_albumArtist;
    QString m_album;
    quint16 m_year;
    unsigned char m_track;
    QString m_genre;
    QHash<QString, QString>* m_ext;


};

#endif // TAG_H
