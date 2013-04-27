#ifndef AUDIODATASERVER_H
#define AUDIODATASERVER_H

#include <QTcpServer>

class AudioDataServer : public QTcpServer
{
    Q_OBJECT
public:
    AudioDataServer(QObject* parent = 0);
    void setFilePath(const QString& path) { filePath = path; }

protected:
    void incomingConnection(int socketDesc);

private:
    QString filePath;

};

#endif // AUDIODATASERVER_H
