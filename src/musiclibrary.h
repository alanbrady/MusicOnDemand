#ifndef MUSICLIBRARY_H
#define MUSICLIBRARY_H

#include <QString>
#include <QSqlDatabase>
#include <QStringList>
#include <QFileInfo>
#include <QObject>
#include <QList>
#include <QPair>
#include "id3tagparser.h"

class MusicLibrary : public QObject {
    Q_OBJECT
public:    
    static const char*      LIBRARYPATH;
    static const char*      CRCDBPATH;
    static const quint16    CRCBITS;
    static const int        MAX_CRCS;
    static const int        MAX_FILES;


//    explicit MusicLibrary(QObject* parent = 0);
    explicit MusicLibrary(QObject* parent = 0,
                          const char* libraryDatabaseConnection = "LIB_DB_CONN",
                          const char* crcDatabaseConnection = "CRC_DB_CONN");
    ~MusicLibrary();

    void addDirectory(const QString& dir, bool recursive = true);
    void removeDirectory(const QString& dir);
    void checkUserDirectories();
    QString getFilePathForId(const QString& id);

    const char* getLibraryConnName() const { return libraryDbConnName; }
    const char* getCrcConnName() const { return crcDbConnName; }

signals:
    void statusUpdate(const QString& newStatus);
    void directoryAdded();
    void directoryRemoved();

private:
    const char* crcDbConnName;
    const char* libraryDbConnName;
    QStringList filesToUpdate;
    QList<QPair<QString, quint16> > crcsToSave;
    unsigned int filesSaved;
    unsigned int filesTotal;
    ID3TagParser parser;

    void checkDirectory(const QString& path);
    void initSql();
    void closeSql();
    QStringList getLibraryDirectories() const;
    quint16 generateDirectoryCRC(const QString& dir) const;
    quint16 generateFileCRC(const QString& path) const;
    quint16 calculateCRC(QDataStream& ds) const;
    quint16 lookupCRC(const QString& dir);
    bool compareCRC(const QString& path, quint16 crc);
    void saveCRC(const QString& path, const quint16 crc);
    void saveCRCs(const QList<QPair<QString, quint16> >& crcs);
    void saveSong(const QString& path);
    void saveSongs(const QStringList& files);


    void catalogDirectory(const QString& dir, const quint16 crc, bool recursive = true);
    void checkFile(const QString& path);
    void checkFiles(const QFileInfoList& files);


    bool createTable(QSqlDatabase db, const QString &name, const QString &fields);


};

#endif // MUSICLIBRARY_H
