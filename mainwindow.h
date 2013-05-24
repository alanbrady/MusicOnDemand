#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSqlQueryModel>
#include "socketinterface.h"
#include "musiclibrarythread.h"
#include "managelibrarydialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

//signals:
//    void addDirectory(const QString& path);
//    void removeDirectory(const QString& path);

public slots:
    void artistListClicked(const QModelIndex& index);
    void albumListClicked(const QModelIndex& index);

private:
    static const char* ARTISTCONN;
    static const char* ALBUMCONN;
    static const char* SONGCONN;

    Ui::MainWindow *ui;
    SocketInterface server;
    MusicLibraryThread libraryThread;
    ManageLibraryDialog* mlDial;
    QSqlQueryModel* artistQuery;
    QSqlQueryModel* albumQuery;
    QSqlQueryModel* songQuery;
    QLabel* statusLabel;

    QString selectedArtist;
    QString selectedAlbum;

    void setAlbumListArist(const QString& artist);
    void setSongListAlbumArtist(const QString& artist, const QString& album);

//    MediaDataServer mediaServer;
//    AudioDataServer audioServer;

};

#endif // MAINWINDOW_H
