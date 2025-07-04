#pragma once
#include "../util.h"

class Server {

protected:
    int port;
    virtual void handleClient(int) = 0;

public:
    Server(int port) : port(port) {}
    virtual void run() = 0;
};
