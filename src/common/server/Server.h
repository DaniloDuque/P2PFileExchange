#pragma once
#include "bytestream/ByteStream.h"

class Server {
protected:
    const int port;
    shared_ptr<ByteStream> stream;
    virtual void handleClient(int) = 0;

public:
    virtual ~Server() = default;
    Server(shared_ptr<ByteStream>& stream, const int port) : port(port), stream(move(stream)) {}

    virtual void run() = 0;
};
