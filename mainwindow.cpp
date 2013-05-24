#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "id3taginterface.h"
#include <QDebug>
#include <QSqlQuery>

const char* MainWindow::ARTISTCONN = "artistConnection";
const char* MainWindow::ALBUMCONN = "albumConnection";
const char* MainWindow::SONGCONN = "songConnection";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusLabel = new QLabel();
    statusLabel->setMaximumWidth(600);
    ui->statusBar->addWidget(statusLabel);
    statusLabel->setText("[MusicOnDemand]");
    server.start();

    mlDial = new ManageLibraryDialog(this);

    // artistQuery setup
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", ARTISTCONN);
    db.setDatabaseName(MusicLibrary::LIBRARYPATH);
    db.open();

    artistQuery = new QSqlQueryModel(this);
    QSqlQuery query("SELECT DISTINCT artist FROM LibraryTable", db);
    artistQuery->setQuery(query);
    ui->artistList->setModel(artistQuery);

    // albumQuery setup
    db = QSqlDatabase::addDatabase("QSQLITE", ALBUMCONN);
    db.setDatabaseName(MusicLibrary::LIBRARYPATH);
    db.open();

    albumQuery = new QSqlQueryModel(this);
    query = QSqlQuery("SELECT DISTINCT album FROM LibraryTable", db);
    albumQuery->setQuery(query);
    ui->albumList->setModel(albumQuery);

    // songQuery setup
    db = QSqlDatabase::addDatabase("QSQLITE", SONGCONN);
    db.setDatabaseName(MusicLibrary::LIBRARYPATH);
    db.open();

    songQuery = new QSqlQueryModel(this);
    query = QSqlQuery("SELECT * FROM LibraryTable", db);
    songQuery->setQuery(query);
    ui->songView->setModel(songQuery);

    selectedArtist = "";
    selectedAlbum = "";

    // connections
    connect(ui->updateButton, SIGNAL(clicked()),
            &libraryThread, SLOT(checkDirectories()));

    connect(&libraryThread, SIGNAL(statusUpdate(QString)),
            statusLabel, SLOT(setText(QString)));

    connect(mlDial, SIGNAL(addDirectory(QString)),
            &libraryThread, SLOT(addDirectory(QString)));

    connect(mlDial, SIGNAL(removeDirectory(QString)),
            &libraryThread, SLOT(removeDirectory(QString)));

    connect(ui->manageButton, SIGNAL(clicked()),
            mlDial, SLOT(show()));

    connect(&libraryThread, SIGNAL(directoryAdded()),
            mlDial, SLOT(slotUpdateModel()));

    connect(&libraryThread, SIGNAL(directoryRemoved()),
            mlDial, SLOT(slotUpdateModel()));

    connect(ui->artistList, SIGNAL(clicked(QModelIndex)),
            this, SLOT(artistListClicked(QModelIndex)));

    connect(ui->albumList, SIGNAL(clicked(QModelIndex)),
            this, SLOT(albumListClicked(QModelIndex)));

}

MainWindow::~MainWindow()
{
    QSqlDatabase db = QSqlDatabase::database(ARTISTCONN);
    db.close();
    db = QSqlDatabase::database(ALBUMCONN);
    db.close();
    db = QSqlDatabase::database(SONGCONN);
    db.close();

    delete ui;
}

void MainWindow::artistListClicked(const QModelIndex &index)
{
    QString artist = artistQuery->data(index).toString();
    selectedArtist = artist;
    selectedAlbum = "";
    setAlbumListArist(artist);
    setSongListAlbumArtist(artist, "");
}

void MainWindow::albumListClicked(const QModelIndex &index)
{
    QString album = albumQuery->data(index).toString();
    selectedAlbum = album;
    setSongListAlbumArtist(selectedArtist, album);
}

void MainWindow::setAlbumListArist(const QString &artist)
{
    QString queryStr = "";
    if (artist == "")
    {
        queryStr = "SELECT DISTINCT album FROM LibraryTable";
    } else {
        queryStr = "SELECT DISTINCT album FROM LibraryTable WHERE artist=\"" +
                   artist + "\"";
    }
    QSqlQuery query(queryStr, QSqlDatabase::database(ARTISTCONN));
    albumQuery->setQuery(query);

}

void MainWindow::setSongListAlbumArtist(const QString &artist, const QString &album)
{
    QString queryStr = "";
    if (artist == "" && album == "") {
        queryStr = "SELECT * FROM LibraryTable";
    } else if (album == "") {
        queryStr = "SELECT * FROM LibraryTable WHERE artist=\"" + artist + "\"";
    } else if (artist == "") {
        queryStr = "SELECT * FROM LibraryTable WHERE album=\"" + album + "\"";
    } else {
        queryStr = "SELECT * FROM LibraryTable WHERE artist=\"" + artist +
                   "\" AND album=\"" + album + "\"";
    }

    QSqlQuery query(queryStr, QSqlDatabase::database(ARTISTCONN));
    songQuery->setQuery(query);
}



