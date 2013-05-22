#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "id3taginterface.h"
#include <QDebug>
#include <QSqlQuery>

const char* MainWindow::artistConn = "artistConnection";
const char* MainWindow::albumConn = "albumConnection";
const char* MainWindow::songConn = "songConnection";

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

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", artistConn);
    db.setDatabaseName(MusicLibrary::LIBRARYPATH);
    db.open();

    artistQuery = new QSqlQueryModel(this);
    QSqlQuery query("SELECT DISTINCT artist FROM LibraryTable", db);
    artistQuery->setQuery(query);
    ui->artistList->setModel(artistQuery);

    db = QSqlDatabase::addDatabase("QSQLITE", albumConn);
    db.setDatabaseName(MusicLibrary::LIBRARYPATH);
    db.open();

    albumQuery = new QSqlQueryModel(this);
    query = QSqlQuery("SELECT DISTINCT album FROM LibraryTable", db);
    albumQuery->setQuery(query);
    ui->albumList->setModel(albumQuery);

    db = QSqlDatabase::addDatabase("QSQLITE", songConn);
    db.setDatabaseName(MusicLibrary::LIBRARYPATH);
    db.open();

    songQuery = new QSqlQueryModel(this);
    query = QSqlQuery("SELECT * FROM LibraryTable", db);
    songQuery->setQuery(query);
    ui->songView->setModel(songQuery);

    connect(ui->updateButton, SIGNAL(clicked()), &libraryThread, SLOT(checkDirectories()));
    connect(&libraryThread, SIGNAL(statusUpdate(QString)), statusLabel, SLOT(setText(QString)));
    connect(mlDial, SIGNAL(addDirectory(QString)), &libraryThread, SLOT(addDirectory(QString)));
    connect(mlDial, SIGNAL(removeDirectory(QString)), &libraryThread, SLOT(removeDirectory(QString)));
    connect(ui->manageButton, SIGNAL(clicked()), mlDial, SLOT(show()));
    connect(&libraryThread, SIGNAL(directoryAdded()), mlDial, SLOT(slotUpdateModel()));
    connect(&libraryThread, SIGNAL(directoryRemoved()), mlDial, SLOT(slotUpdateModel()));
}

MainWindow::~MainWindow()
{
    QSqlDatabase db = QSqlDatabase::database(artistConn);
    db.close();
    db = QSqlDatabase::database(albumConn);
    db.close();
    db = QSqlDatabase::database(songConn);
    db.close();

    delete ui;
}

void MainWindow::setAlbumListArist(const QString &artist)
{
}

void MainWindow::setSongListAlbumArtist(const QString &artist, const QString &album)
{
}



