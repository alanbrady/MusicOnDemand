#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSqlQueryModel>
#include <QSqlTableModel>
//#include "socketinterface.h"
#include "musiclibrarythread.h"
#include "managelibrarydialog.h"
#include "serverfactory.h"

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
    void libraryUpdateComplete();

private:
    static const char* ARTISTCONN;
    static const char* ALBUMCONN;
    static const char* SONGCONN;

    Ui::MainWindow *ui;
//    SocketInterface server;
    ServerInterface* audioDataServer;
    ServerInterface* mediaDataServer;
    MusicLibraryThread libraryThread;
    ManageLibraryDialog* mlDial;
    QSqlQueryModel* artistQuery;
    QSqlQueryModel* albumQuery;
    QSqlTableModel* songTableModel;
    QLabel* statusLabel;

    QString selectedArtist;
    QString selectedAlbum;

    void setAlbumListArist(const QString& artist);
    void setSongListAlbumArtist(const QString& artist, const QString& album);
    void resetQueries();

//    MediaDataServer mediaServer;
//    AudioDataServer audioServer;

};

#endif // MAINWINDOW_H
