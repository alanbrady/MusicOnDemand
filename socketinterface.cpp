#include "socketinterface.h"

SocketInterface::SocketInterface()
    : started(false), audioPort(50008), mediaPort(50009)
{
}

void SocketInterface::start() {
    if (!started) {
        started = startAudioDataServer() && startMediaDataServer();
    }
}

void SocketInterface::stop() {
    if (started) {
        stopAudioDataServer();
        stopMediaDataServer();
        started = false;
    }
}

bool SocketInterface::startAudioDataServer() {
    if (!audioServer.isListening())
        return audioServer.listen(QHostAddress::Any, audioPort);
    else return false;
}

bool SocketInterface::startMediaDataServer() {
    if (!mediaServer.isListening())
        return mediaServer.listen(QHostAddress::Any, mediaPort);
    else return false;
}

void SocketInterface::stopAudioDataServer() {
    audioServer.close();
}

void SocketInterface::stopMediaDataServer() {
    mediaServer.close();
}

bool SocketInterface::setAudioPort(quint16 port) {
    audioPort = port;
    stopAudioDataServer();
    return startAudioDataServer();
}

bool SocketInterface::setMediaPort(quint16 port) {
    mediaPort = port;
    stopMediaDataServer();
    return startMediaDataServer();
}
