#ifndef TCP_SERVER_HEADER
#define TCP_SERVER_HEADER

#include "../util.h"

class TCPServer {

protected:
    int port;
    virtual void handleClient(int) = 0;

public:
    TCPServer(int);
    void run();
};

#include "TCPServer.cpp"

#endif
