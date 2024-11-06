#ifndef PEER_SERVER_HEADER
#define PEER_SERVER_HEADER

#include "../util.h"
#include "../Network/TCPServer.h"

class PeerServer : public TCPServer {

private:

    void handleClient(int) override;

public:

    PeerServer(int);



};

#include "PeerServer.cpp"

#endif
