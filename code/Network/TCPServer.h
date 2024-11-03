#ifndef TCPSERVER_HEADER
#define TCPSERVER_HEADER

#include "../util.h"

class TCPServer {

private:
    int port;
    void handleClient(int);

public:
    TCPServer(int);
    void run();
};

#include "TCPServer.cpp"

#endif
