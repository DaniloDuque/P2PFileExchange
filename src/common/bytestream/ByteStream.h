#pragma once
#include <string>
using namespace std;

class ByteStream {
public:
    ByteStream() = default;

    virtual ~ByteStream() = default;

    virtual pair<bool, string> read(int) = 0;

    virtual bool write(bool, int, const string &) = 0;

    virtual bool send_acknowledge(int) = 0;

    virtual bool receive_acknowledge(int) = 0;
};
