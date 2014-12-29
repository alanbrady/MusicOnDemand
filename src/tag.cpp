#include "tag.h"

Tag::Tag()
{
    m_valid = false;
    m_title = "Unknown";
    m_artist = "Unknown";
    m_albumArtist = "Unknown";
    m_album = "Unknown";
    m_year = 0;
    m_track = 0;
    m_genre = "";
    m_ext = 0;
}

Tag::~Tag() {
    if (m_ext != 0)
        delete m_ext;
}

Tag::Tag(const Tag &other)
{
    m_valid = other.m_valid;
    m_title = other.m_title;
    m_artist = other.m_artist;
    m_albumArtist = other.m_albumArtist;
    m_album = other.m_album;
    m_year = other.m_year;
    m_track = other.m_track;
    m_genre = other.m_genre;

    if (other.m_ext != 0) {
        QHash<QString, QString>* extPtr = other.m_ext;
        m_ext = new QHash<QString, QString>(*extPtr);
    } else m_ext = 0;
}

Tag &Tag::operator =(const Tag &other)
{
    m_valid = other.m_valid;
    m_title = other.m_title;
    m_artist = other.m_artist;
    m_albumArtist = other.m_albumArtist;
    m_album = other.m_album;
    m_year = other.m_year;
    m_track = other.m_track;
    m_genre = other.m_genre;

    if (other.m_ext != 0) {
        QHash<QString, QString>* extPtr = other.m_ext;
        m_ext = new QHash<QString, QString>(*extPtr);
    } else m_ext = 0;
    return *this;
}

void Tag::setTitle(const QString &title)
{
    if (!title.isEmpty())
        m_valid = true;

    m_title = title;
}

void Tag::setArtist(const QString &artist)
{
    if (!artist.isEmpty())
        m_valid = true;

    m_artist = artist;
}

void Tag::setAlbumArtist(const QString &albumArtist)
{
    if (!albumArtist.isEmpty())
        m_valid = true;

    m_albumArtist = albumArtist;
}

void Tag::setAlbum(const QString &album)
{
    if (!album.isEmpty())
        m_valid = true;

    m_album = album;
}

void Tag::setYear(const quint16& year)
{
    if (year != 0)
        m_valid = true;

    m_year = year;
}

void Tag::setTrack(const unsigned char track)
{
    if (track != 0)
        m_valid = true;

    m_track = track;
}

void Tag::setGenre(const unsigned char genre)
{
    if (genre != 255)
        m_valid = true;

    m_genre = QString() + genre;
}

void Tag::setGenre(const QString &genre)
{
    if (!genre.isEmpty())
        m_valid = true;

    m_genre = genre;
}

void Tag::addExtendedData(const QString &key, const QString &value)
{
    if (m_ext == 0)
        m_ext = new QHash<QString, QString>();

    m_ext->insert(key, value);
}
