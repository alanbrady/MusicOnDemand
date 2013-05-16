#ifndef MEDIADATASERVER_H
#define MEDIADATASERVER_H

#include <QTcpServer>
#include "mediasocketthread.h"
//#include "id3taginterface.h"
#include "tag.h"
#include "id3tagparser.h"

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
    ID3TagParser parser;

    QByteArray getData(MESSAGE m, QString msg) const;
    QByteArray getLibraryLastModified() const;
    QByteArray getLibraryDatabase() const;
    QByteArray getAlbumArt(const QString& filePath) const;
    QByteArray getID3Tags(const QString& filePath) const;
    QByteArray tagToBytes(Tag tag) const;
};

#endif // MEDIADATASERVER_H
