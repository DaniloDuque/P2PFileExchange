#pragma once
#include "bytestream/ByteStream.h"

class Server {
protected:
    const int port;
    const shared_ptr<ByteStream> stream;
    virtual void handle_client(int) = 0;

public:
    virtual ~Server() = default;
    Server(const shared_ptr<ByteStream>& stream, const int port) : port(port), stream(stream) {}
    virtual void run() = 0;
    virtual void stop() = 0;
};
