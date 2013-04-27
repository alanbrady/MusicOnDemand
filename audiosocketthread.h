#ifndef AUDIOSOCKETTHREAD_H
#define AUDIOSOCKETTHREAD_H

#include <QThread>
#include <QTcpSocket>

class AudioSocketThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioSocketThread(const int socketDesc, QObject *parent = 0);
    ~AudioSocketThread() { delete socket; }

public slots:
    void slotReadyRead();
    void sendData(QByteArray dataToSend);

protected:
    void run();

signals:
    void error(QTcpSocket::SocketError socketError);

private:
    const int m_sock;
    QTcpSocket* socket;

    QByteArray readFile(const QString& path);

};

#endif // AUDIOSOCKETTHREAD_H
