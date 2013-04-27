#ifndef SOCKETINTERFACE_H
#define SOCKETINTERFACE_H

#include <QString>
#include "mediadataserver.h"
#include "audiodataserver.h"

class SocketInterface {
public:
    SocketInterface();

    bool setAudioPort(quint16 port);
    bool setMediaPort(quint16 port);
    bool isStarted() const { return started; }

public slots:
    void start();
    void stop();

private:
    bool started;
    MediaDataServer mediaServer;
    AudioDataServer audioServer;
    quint16 audioPort;
    quint16 mediaPort;

    bool startAudioDataServer();
    void stopAudioDataServer();
    bool startMediaDataServer();
    void stopMediaDataServer();

};



#endif
