#ifndef MUSICLIBRARYTHREAD_H
#define MUSICLIBRARYTHREAD_H

#include <QThread>
//#include <QMutex>
#include "musiclibrary.h"

class MusicLibraryThread : public QThread
{
    Q_OBJECT
public:
    explicit MusicLibraryThread(QObject *parent = 0);
    ~MusicLibraryThread();

    const QString getLibraryDbConn() const { return library->getLibraryConnName(); }
    const QString getCrcDbConn() const { return library->getCrcConnName(); }

signals:
    void statusUpdate(const QString& newStatus);
    void directoryAdded();
    void directoryRemoved();
    void updateComplete();

public slots:
    void checkDirectories();
    void addDirectory(const QString& path);
    void removeDirectory(const QString& path);

private slots:
    void slotStatusUpdate(const QString& newStatus);
    void slotDirectoryAdded();
    void slotDirectoryRemoved();

protected:
    void run();

private:
    MusicLibrary* library;
//    QMutex mutex;

};

#endif // MUSICLIBRARYTHREAD_H
