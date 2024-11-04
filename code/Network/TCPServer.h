#ifndef TCPSERVER_HEADER
#define TCPSERVER_HEADER

#include "../util.h"

class TCPServer {

private:
    int port;
    virtual void handleClient(int) const;

public:
    TCPServer(int);
    void run();
};

#include "TCPServer.cpp"

#endif
