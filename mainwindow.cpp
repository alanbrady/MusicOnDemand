#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "id3taginterface.h"
#include <QDebug>

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
    delete ui;
}

