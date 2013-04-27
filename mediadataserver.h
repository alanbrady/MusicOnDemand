#ifndef MEDIADATASERVER_H
#define MEDIADATASERVER_H

#include <QTcpServer>
#include "mediasocketthread.h"
#include "id3taginterface.h"

class MediaDataServer : public QTcpServer {
    Q_OBJECT
public:    
    MediaDataServer(QObject* parent = 0);

    enum MESSAGE {
        LIB_MOD_DATE_REQ =  0x01,
        LIB_FILE_REQ =      0x02,
        ALBUM_ART_REQ =     0x04,
        ID3_REQ =           0x08
    };

protected:
    void incomingConnection(int socketDesc);

private slots:
    void receiveMessage(MediaSocketThread* thread, QByteArray msgData);

private:
    QTcpSocket* sock;

    QByteArray getData(MESSAGE m, QString msg) const;
    QByteArray getLibraryLastModified() const;
    QByteArray getLibraryDatabase() const;
    QByteArray getAlbumArt(const QString& filePath) const;
    QByteArray getID3Tags(const QString& filePath) const;
    QByteArray convertQTagsToBytes(const ID3TagInterface::QTags& tags) const;
};

#endif // MEDIADATASERVER_H
