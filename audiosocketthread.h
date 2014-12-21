#ifndef AUDIOSOCKETTHREAD_H
#define AUDIOSOCKETTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include "musiclibrary.h"

class AudioSocketThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioSocketThread(const int socketDesc, QObject *parent = 0);
    ~AudioSocketThread() { delete m_socket; }

public slots:
    void slotReadyRead();
    void sendData(QByteArray dataToSend);

protected:
    void run();

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    const int m_sock;
    QTcpSocket* m_socket;
    MusicLibrary musicLibrary;

    QByteArray readFile(const QString& path);
    QString getFilePathForID(const QString& id);

};

#endif // AUDIOSOCKETTHREAD_H
