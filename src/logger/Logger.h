#pragma once
#include <string>
using namespace std;

class Logger {
public:
    void warn(const string&) const;
    void info(const string&) const;
    void error(const string&) const;
    void debug(const string&) const;
};

extern Logger logger;