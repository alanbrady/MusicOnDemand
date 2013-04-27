#include "musiclibrarythread.h"

MusicLibraryThread::MusicLibraryThread(QObject *parent) :
    QThread(parent)
{
    library = new MusicLibrary(this);
    connect(library, SIGNAL(statusUpdate(QString)), this, SLOT(slotStatusUpdate(QString)));
    connect(library, SIGNAL(directoryAdded()), this, SLOT(slotDirectoryAdded()));
    connect(library, SIGNAL(directoryRemoved()), this, SLOT(slotDirectoryRemoved()));
    QObject::moveToThread(this);

    this->start();
}

MusicLibraryThread::~MusicLibraryThread() {
    this->quit();
    this->wait();
}

void MusicLibraryThread::slotStatusUpdate(const QString &newStatus) {
    emit statusUpdate(newStatus);
}

void MusicLibraryThread::checkDirectories() {
    library->checkUserDirectories();
}

void MusicLibraryThread::addDirectory(const QString &path) {
    library->addDirectory(path);
}

void MusicLibraryThread::removeDirectory(const QString &path) {
    library->removeDirectory(path);
}

void MusicLibraryThread::run() {
    this->exec();
}

void MusicLibraryThread::slotDirectoryAdded() {
    emit directoryAdded();
}

void MusicLibraryThread::slotDirectoryRemoved() {
    emit directoryRemoved();
}
