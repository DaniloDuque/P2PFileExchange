#include "Logger.h"
#include <iostream>
using namespace std;

Logger logger;

void Logger::info(string message) {
    cout << "\033[32m[INFO]\033[0m " << message << '\n';
}

void Logger::error(string message) {
    cerr << "\033[31m[ERROR]\033[0m " << message << '\n';
}

void Logger::debug(string message) {
    cout << "\033[36m[DEBUG]\033[0m " << message << '\n';
}

void Logger::warn(string message) {
    cout << "\033[33m[WARN]\033[0m " << message << '\n';
}