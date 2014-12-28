#ifndef MEDIADATASERVER_H
#define MEDIADATASERVER_H

#include <QTcpServer>
//#include "mediasocketthread.h"
#include "tag.h"
#include "id3tagparser.h"
#include "serverinterface.h"

class MediaDataServer : public ServerInterface {
    Q_OBJECT
public:
    MediaDataServer(quint16 port);

    void parseMessage(const char *data, const unsigned int dataLen,
                      QTcpSocket *sender);
private:
    enum Message {
        LIB_MOD_DATE_REQ    = 0x01,
        LIB_FILE_REQ        = 0x02,
        ALBUM_ART_REQ       = 0X04,
        ID3_REQ             = 0X08
    };

    ID3TagParser m_id3Parser;

    QByteArray getReturnData(Message msg, const QString &requestData) const;
    QByteArray getLibraryLastModified() const;
    QByteArray getLibraryDatabase() const;
    QByteArray getAlbumArt(const QString& filePath) const;
    QByteArray getID3Tags(const QString& filePath) const;
    QByteArray tagToBytes(Tag tag) const;

};

#endif // MEDIADATASERVER_H
