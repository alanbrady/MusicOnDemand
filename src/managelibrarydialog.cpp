#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include "managelibrarydialog.h"
#include "ui_managelibrarydialog.h"
#include "musiclibrary.h"

const char ManageLibraryDialog::manageLibConn[] =   "MANAGE_LIB_CONN";
const char ManageLibraryDialog::libraryQuery[] =    "SELECT * FROM UserDirectories";

ManageLibraryDialog::ManageLibraryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageLibraryDialog)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", manageLibConn);
    db.setDatabaseName(MusicLibrary::CRCDBPATH);
    db.open();

    QSqlQuery query(libraryQuery, db);
    libraryModel = new QSqlQueryModel();
    libraryModel->setQuery(query);
    libraryModel->setHeaderData(0, Qt::Horizontal, tr("Directories Selected for Library"));

    ui->userDirectoriesListView->setModel(libraryModel);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(slotAddDirectory()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveDirectory()));
}

ManageLibraryDialog::~ManageLibraryDialog()
{
    delete ui;
    {
        QSqlDatabase db = QSqlDatabase::database(manageLibConn);
        if (db.isOpen())
            db.close();
    }
    QSqlDatabase::removeDatabase(manageLibConn);
}


void ManageLibraryDialog::slotAddDirectory() {
    QString dirToAdd = QFileDialog::getExistingDirectory(this, "Add Directory to Library", "/");
    if (dirToAdd.length() > 0) {
        emit addDirectory(dirToAdd);
    }
}

void ManageLibraryDialog::slotRemoveDirectory() {
    QModelIndex i = ui->userDirectoriesListView->currentIndex();
    QString dirToRemove = libraryModel->record(i.row()).value("path").toString();
    if (QMessageBox::question(this,
                              tr("Remove Directory"),
                              QString("Remove %1 from database?").arg(dirToRemove),
                              QMessageBox::Yes,
                              QMessageBox::Cancel) == QMessageBox::Yes) {

        emit removeDirectory(dirToRemove);
    }
}

void ManageLibraryDialog::slotUpdateModel() {
    libraryModel->setQuery(libraryQuery, QSqlDatabase::database(manageLibConn));
}
