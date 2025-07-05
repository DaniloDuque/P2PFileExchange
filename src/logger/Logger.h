#pragma once
#include <string>
using namespace std;

class Logger {
public:
    void warn(string);
    void info(string);
    void error(string);
    void debug(string);
};

extern Logger logger;