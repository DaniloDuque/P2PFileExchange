#pragma once
#include <string>
#include <mutex>
using namespace std;

class Logger {
    mutable mutex outputMutex;
public:
    void warn(const string&) const;
    void info(const string&) const;
    void error(const string&) const;
    void debug(const string&) const;
};

extern Logger logger;