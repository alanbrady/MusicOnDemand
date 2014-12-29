#ifndef AUDIODATASERVER_H
#define AUDIODATASERVER_H

//#include <QTcpServer>
#include "serverinterface.h"
#include "musiclibrary.h"

class AudioDataServer : public ServerInterface
{
    Q_OBJECT
public:
    AudioDataServer(quint16 port);

protected:
    void parseMessage(const char * msg, const unsigned int msgLen,
                      QTcpSocket * sender);

private:
    MusicLibrary musicLibrary;

    QByteArray readFile(const QString& path);
    QString getFilePathForID(const QString& id);
};

#endif // AUDIODATASERVER_H
