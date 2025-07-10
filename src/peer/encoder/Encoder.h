#pragma once
#include <string>
using namespace std;

class Encoder {
public:
    virtual ~Encoder() = default;
    virtual string encode(const string&) = 0;
    virtual string decode(const string&) = 0;
};
