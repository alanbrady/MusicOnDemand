#include "mainwindow.h"
#include <QDebug>
#include <QSqlQuery>
#include <QHeaderView>
#include "ui_mainwindow.h"
#include "id3taginterface.h"

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
//    server.start();

    audioDataServer = ServerFactory::createAudioDataServer(50008);
    mediaDataServer = ServerFactory::createMediaDataServer(50009);

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

    // songTableModel setup
    db = QSqlDatabase::addDatabase("QSQLITE", SONGCONN);
    db.setDatabaseName(MusicLibrary::LIBRARYPATH);
    db.open();

    songTableModel = new QSqlTableModel(this, db);
    songTableModel->setTable("LibraryTable");
    songTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    songTableModel->select();
    songTableModel->removeColumns(0, 2);
    songTableModel->removeColumn(4);
    songTableModel->setHeaderData(0, Qt::Horizontal, "Artist");
    songTableModel->setHeaderData(1, Qt::Horizontal, "Album");
    songTableModel->setHeaderData(2, Qt::Horizontal, "Title");
    songTableModel->setHeaderData(3, Qt::Horizontal, "Track");
    ui->songView->setModel(songTableModel);

    // enable sorting
    QHeaderView* header = ui->songView->horizontalHeader();
    header->setSortIndicator(2, Qt::AscendingOrder);
    header->setSectionsMovable(true);
    header->setSortIndicatorShown(true);
    header->setSectionsClickable(true);
    header->setStretchLastSection(true);
    ui->songView->setSortingEnabled(true);

    selectedArtist = "";
    selectedAlbum = "";

    // signal/slot connections
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

    connect(&libraryThread, SIGNAL(updateComplete()),
            this, SLOT(libraryUpdateComplete()));

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

    delete audioDataServer;
    delete mediaDataServer;
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

void MainWindow::libraryUpdateComplete()
{
    resetQueries();
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
    QString filterStr = "";
    if (artist == "" && album == "") {
        filterStr = "";
    } else if (album == "") {
        filterStr = "artist=\"" + artist + "\"";
    } else if (artist == "") {
        filterStr = "album=\"" + album + "\"";
    } else {
        filterStr = "artist=\"" + artist + "\" AND album=\"" + album + "\"";
    }

    songTableModel->setFilter(filterStr);
}

void MainWindow::resetQueries()
{
    QSqlQuery query("SELECT DISTINCT artist FROM LibraryTable",
                    QSqlDatabase::database(ARTISTCONN));

    artistQuery->setQuery(query);
    setAlbumListArist(selectedArtist);
    setSongListAlbumArtist(selectedArtist, selectedAlbum);
}



