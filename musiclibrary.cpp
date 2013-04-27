#include "musiclibrary.h"
#include <QByteArray>
#include <QDir>
#include <QBuffer>
#include <QDebug>
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include <QThread>
#include "id3taginterface.h"

const char* MusicLibrary::LIBRARYPATH =     "md_library.db";
const char* MusicLibrary::CRCDBPATH =       "md_crc.db";
const quint16 MusicLibrary::CRCBITS =       0x8005;
const int MusicLibrary::MAX_CRCS =          1000;
const int MusicLibrary::MAX_FILES =         1000;


MusicLibrary::MusicLibrary(QObject* parent)
    : QObject(parent), crcDbConnName("CRC_DB_CONN"), libraryDbConnName("LIB_DB_CONN")
{
    initSql();
}

MusicLibrary::MusicLibrary(const char *crcDatabaseConnection, const char *libraryDatabaseConnection, QObject* parent)
    : QObject(parent), crcDbConnName(crcDatabaseConnection), libraryDbConnName(libraryDatabaseConnection)
{
    initSql();
}

MusicLibrary::~MusicLibrary() {
    closeSql();
}

// Check the user selected directories for songs
void MusicLibrary::checkUserDirectories(){
    emit statusUpdate("Updating library...");
    filesSaved = 0;
    filesTotal = 0;
    QStringList dirs = getLibraryDirectories();
    for (int i = 0; i < dirs.size(); i++) {
        this->checkDirectory(dirs.at(i));
    }

    // batch save all new crcs
    saveCRCs(crcsToSave);
    crcsToSave.clear();

    // save songs to sql database
    saveSongs(filesToUpdate);
    filesToUpdate.clear();
    emit statusUpdate("Library update complete");
//    qDebug() << "Files saved: " << filesSaved << " Files found: " << filesTotal;
    emit statusUpdate(QString("Update complete. Files updated: %1").arg(filesSaved));
}

// Check a directory's crc and update it if crc not found or different
void MusicLibrary::checkDirectory(const QString& path) {
//    emit statusUpdate(QString("Updating directory: %1").arg(path));
    if (QDir(path).exists()) {
        quint16 crc = generateDirectoryCRC(path);
        if (!compareCRC(path, crc)) {
            this->catalogDirectory(path, crc);
        }
    } else removeDirectory(path);
}

// Make a crc for a given directory
quint16 MusicLibrary::generateDirectoryCRC(const QString& dir) const {
    QDir d(dir);
    if (d.exists()) {
        QFileInfoList files = d.entryInfoList();
        QByteArray data;
        QDataStream ds(&data, QIODevice::ReadWrite);
        ds.setVersion(QDataStream::Qt_4_0);

        for (int i = 0; i < files.size(); i++) {
            QFileInfo file = files.at(i);
            QString filePath = file.canonicalFilePath();
            ds.writeRawData(filePath.toLocal8Bit(), filePath.size());
            ds << file.size();
            ds << file.permissions();
            ds << file.lastModified();
        }

        return calculateCRC(ds);

    } else return 0;
}

// Construct a datastream of a file, give to crc func
quint16 MusicLibrary::generateFileCRC(const QString &path) const {
    QFileInfo fi(path);
    if (fi.exists()) {
        QByteArray data;
        QDataStream ds(&data, QIODevice::ReadWrite);
        ds.setVersion(QDataStream::Qt_4_0);
        QString filePath;

        filePath = fi.canonicalFilePath();
        ds.writeRawData(filePath.toLocal8Bit(), filePath.size());
        ds << fi.size();
        ds << fi.permissions();
        ds << fi.lastModified();

        return calculateCRC(ds);

    } else return 0;
}

// Main crc generator func, given a data stream make a crc
quint16 MusicLibrary::calculateCRC(QDataStream& ds) const {
    ds.device()->seek(0);
    quint32 dbits;
    ds >> dbits;
    int j = 0;
    while(true) {
        for (int i = 0; i < 8; i++) {
            bool lbitIsOne = dbits & 0x80000000;
            dbits = dbits << 1;
            if (lbitIsOne) {
                dbits = dbits ^ (CRCBITS << 16);
            }
            if (ds.atEnd()) {
                if (j > 16 && (quint16)(dbits >> j) == 0)
                    return (quint16)(dbits >> (j-16));
                j++;
            }
        }
        if (!ds.atEnd()) {
            quint8 nextByte;
            ds >> nextByte;
            dbits = dbits ^ nextByte;
        }
    }
}

// Find old crc in database, compare against new crc
bool MusicLibrary::compareCRC(const QString& path, quint16 crc){
    quint16 oldCrc = lookupCRC(path);
    if (oldCrc == crc)
        return true;
    return false;
}

void MusicLibrary::catalogDirectory(const QString &dir, const quint16 crc, bool recursive){
    crcsToSave.append(qMakePair<QString, quint16>(dir, crc));
    if (crcsToSave.size() > MAX_CRCS) {
        saveCRCs(crcsToSave);
        crcsToSave.clear();
    }

    QDir d(dir);
    QFileInfoList dirs = d.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
    for (int i = 0; i < dirs.size(); i++) {
        this->checkDirectory(dirs.at(i).canonicalFilePath());
    }

    QStringList nameFilters;
    nameFilters << "*.mp3" << "*.ogg" << "*.flac" << "*.wav";
    QFileInfoList files = d.entryInfoList(nameFilters, QDir::Files);
    checkFiles(files);
}

void MusicLibrary::checkFile(const QString &path) {
    filesTotal++;
    filesToUpdate.append(path);
}

void MusicLibrary::checkFiles(const QFileInfoList &files) {
    for (int i = 0; i < files.size(); i++) {
        checkFile(files.at(i).canonicalFilePath());
        qDebug() << QFile::encodeName(files.at(i).canonicalFilePath().toUtf8());
    }
    if (filesToUpdate.size() > MAX_FILES) {
        saveSongs(filesToUpdate);
        filesToUpdate.clear();
    }
}


// ************ SQL ************* //

void MusicLibrary::initSql() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", libraryDbConnName);
    db.setDatabaseName(LIBRARYPATH);
    db.open();
    this->createTable(db, "LibraryTable",
                      "filePath TEXT PRIMARY KEY, fileSize INTEGER, artist TEXT, album TEXT, track TEXT, tracknum INTEGER, date INTEGER");

    db = QSqlDatabase::addDatabase("QSQLITE", crcDbConnName);
    db.setDatabaseName(CRCDBPATH);
    db.open();
    this->createTable(db, "CRCTable", "path TEXT PRIMARY KEY, crc INTEGER");
    this->createTable(db, "UserDirectories", "path TEXT PRIMARY KEY");
}

void MusicLibrary::closeSql() {
    QSqlDatabase db = QSqlDatabase::database(libraryDbConnName);
    if (db.isOpen())
        db.close();
    db = QSqlDatabase::database(crcDbConnName);
    if (db.isOpen())
        db.close();
}

bool MusicLibrary::createTable(QSqlDatabase db, const QString &name, const QString &fields) {
    QSqlQuery query(db);
    QString qs = "CREATE TABLE IF NOT EXISTS ";
    qs += name;
    qs += "(";
    qs += fields;
    qs += ")";
    return query.exec(qs);
}

QStringList MusicLibrary::getLibraryDirectories() const{
    QStringList dirs;
    QSqlQuery query(QSqlDatabase::database(crcDbConnName));
    query.prepare("SELECT * FROM UserDirectories");
    query.exec();
    while (query.next()) {
        dirs << query.value(0).toString();
    }
    return dirs;
}

quint16 MusicLibrary::lookupCRC(const QString& dir){
    QSqlQuery query(QSqlDatabase::database(crcDbConnName));
    QString stmt = "SELECT crc FROM CRCTable WHERE path='%1'";
    query.prepare(stmt.arg(dir));
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else return 0;
}

void MusicLibrary::addDirectory(const QString& dir, bool recursive){
    QSqlQuery query(QSqlDatabase::database(crcDbConnName));
    QString stmt = "INSERT INTO UserDirectories (path) VALUES ('%1')";
    query.prepare(stmt.arg(dir));
    if (query.exec()) {
//        emit statusUpdate(QString("Added directory: %1").arg(dir));
        emit directoryAdded();
//    } else {
//        emit statusUpdate(QString("Failed to add directory: %1").arg(dir));
    }
}

void MusicLibrary::removeDirectory(const QString& dir){
    QSqlQuery query(QSqlDatabase::database(crcDbConnName));
    QString stmt = "DELETE FROM UserDirectories WHERE path='%1'";
    query.prepare(stmt.arg(dir));
    if (query.exec()) {
//        emit statusUpdate(QString("Removed directory: %1").arg(dir));
        emit directoryRemoved();
//    } else {
//        emit statusUpdate(QString("Failed to remove directory: %1").arg(dir));
    }
}

void MusicLibrary::saveCRC(const QString& path, const quint16 crc){
    QSqlQuery query(QSqlDatabase::database(crcDbConnName));
    QString stmt = "INSERT OR REPLACE INTO CRCTable (path, crc) VALUES('%1', %2)";
    query.exec(stmt.arg(path).arg(crc));
}

void MusicLibrary::saveCRCs(const QList<QPair<QString, quint16> > &crcs) {
//    emit statusUpdate("Saving crcs...");
    QSqlQuery query(QSqlDatabase::database(crcDbConnName));
    query.exec("BEGIN;");
    QString stmt = "INSERT OR REPLACE INTO CRCTable (path, crc) VALUES('%1', %2)";
    for (int i = 0; i < crcs.size(); i++) {
        query.exec(stmt.arg(crcs.at(i).first).arg(crcs.at(i).second));
    }
    query.exec("END;");
}

void MusicLibrary::saveSong(const QString& path) {
    ID3TagInterface::QTagMap tags = ID3TagInterface::getTagMap(path.toUtf8());
    QSqlQuery query(QSqlDatabase::database(libraryDbConnName));
    QFileInfo fi(path);
    query.prepare("INSERT OR REPLACE INTO LibraryTable (filePath, fileSize, artist, album, track, tracknum, date) VALUES(:filePath, :fileSize, :artist, :album, :track, :tracknum, :date)");
    query.bindValue(":filePath", fi.canonicalFilePath());
    query.bindValue(":fileSize", fi.size());
    query.bindValue(":artist", tags.value("Artist"));
    query.bindValue(":album", tags.value("Album"));
    query.bindValue(":track", tags.value("Track"));
    query.bindValue(":tracknum", tags.value("TrackNum"));
    query.bindValue(":date", QDateTime::currentMSecsSinceEpoch());
    query.exec();

}

void MusicLibrary::saveSongs(const QStringList& files) {
    ID3TagInterface::QTagMap tags;
    QSqlQuery query(QSqlDatabase::database(libraryDbConnName));
    QString stmt("INSERT OR REPLACE INTO LibraryTable (filePath, fileSize, artist, album, track, tracknum, date) VALUES('%1', %2, '%3', '%4', '%5', %6, %7)");
    query.exec("BEGIN;");
    for (int i = 0; i < files.size(); i++) {
        filesSaved++;
//        emit statusUpdate(QString("(%1 of %2)Saving song: %3").arg(filesSaved).arg(filesTotal).arg(files.at(i)));
        tags = ID3TagInterface::getTagMap(files.at(i));
        query.exec(stmt.arg(files.at(i))
                   .arg(QFile(files.at(i)).size())
                   .arg(tags.value("Artist"))
                   .arg(tags.value("Album"))
                   .arg(tags.value("Track"))
                   .arg(tags.value("TrackNum"))
                   .arg(QDateTime::currentMSecsSinceEpoch())
                   );
    }
    query.exec("END;");
}
