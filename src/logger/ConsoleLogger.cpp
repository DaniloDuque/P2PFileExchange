#include "Logger.h"
#include <iostream>
#include "constants.h"
using namespace std;

Logger logger;

void Logger::info(const string& message) const {
    lock_guard lock(outputMutex);
    cout << GREEN << INFO << " " << RESET_COLOR << message << '\n';
}

void Logger::error(const string& message) const {
    lock_guard lock(outputMutex);
    cerr << RED << ERROR << " " << RESET_COLOR << message << '\n';
}

void Logger::debug(const string& message) const {
    lock_guard lock(outputMutex);
    cout << BLUE << DEBUG << " " << RESET_COLOR << message << '\n';
}

void Logger::warn(const string& message) const {
    lock_guard lock(outputMutex);
    cout << YELLOW << WARN << " " << RESET_COLOR << message << '\n';
}
