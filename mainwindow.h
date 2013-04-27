#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
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

private:
    Ui::MainWindow *ui;
    SocketInterface server;
    MusicLibraryThread libraryThread;
    ManageLibraryDialog* mlDial;

    QLabel* statusLabel;
//    MediaDataServer mediaServer;
//    AudioDataServer audioServer;

};

#endif // MAINWINDOW_H
