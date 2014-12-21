#ifndef MEDIASOCKETTHREAD_H
#define MEDIASOCKETTHREAD_H

#include <QThread>
#include <QByteArray>
#include <QTcpSocket>
#include <QDataStream>

class MediaSocketThread : public QThread {
    Q_OBJECT
public:
    MediaSocketThread(int socketDesc);
    ~MediaSocketThread();

public slots:
    void slotReadyRead();
    void sendData(QByteArray dataToSend);

signals:
    void error(QAbstractSocket::SocketError error);
    void messageReceived(MediaSocketThread* thread, QByteArray msgData);

protected:
    void run();

private:
    const int m_sock;
    QTcpSocket* m_socket;

};

#endif // MEDIASOCKETTHREAD_H
