#ifndef SERVERFACTORY_H
#define SERVERFACTORY_H

#include <list>
#include "serverinterface.h"
#include "audiodataserver.h"
#include "mediadataserver.h"

class ServerFactory
{
public:
    ServerFactory();

    static ServerInterface* createAudioDataServer(quint16 port) {
        return new AudioDataServer(port);
    }

    static ServerInterface* createMediaDataServer(quint16 port) {
        return new MediaDataServer(port);
    }

private:

};

#endif // SERVERFACTORY_H
