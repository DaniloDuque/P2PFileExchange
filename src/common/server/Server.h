#pragma once

class Server {
protected:
    const int port;
    virtual void handleClient(int) = 0;

public:
    virtual ~Server() = default;
    explicit Server(const int port) : port(port) {}
    virtual void run() = 0;
};
