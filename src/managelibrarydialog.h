#ifndef MANAGELIBRARYDIALOG_H
#define MANAGELIBRARYDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
    class ManageLibraryDialog;
}

class ManageLibraryDialog : public QDialog
{
    Q_OBJECT

public:
    static const char manageLibConn[];
    static const char libraryQuery[];

    explicit ManageLibraryDialog(QWidget *parent = 0);
    ~ManageLibraryDialog();

signals:
    void addDirectory(const QString&);
    void removeDirectory(const QString&);

public slots:
    void slotAddDirectory();
    void slotRemoveDirectory();
    void slotUpdateModel();

private:
    Ui::ManageLibraryDialog *ui;
    QSqlQueryModel* libraryModel;
};

#endif // MANAGELIBRARYDIALOG_H
