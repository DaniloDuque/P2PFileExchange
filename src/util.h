#pragma once
#include <iostream>
#include <thread>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <sys/socket.h>
#include <cstring>
#include <algorithm>
#include <set>
#include <iomanip>
#include "logger/Logger.h"
#include "constants.h"

#define ll long long
#define uchar unsigned char
using namespace std;

inline string readSingleBuffer(const int socket) {
    char buffer[BUFFER_SIZE] = {};
    const ssize_t bytesRead = recv(socket, buffer, BUFFER_SIZE, 0);
    if (bytesRead < 0) {
        logger.error("Error in readSingleBuffer: " + string(strerror(errno)));
        return "";
    }
    return string(buffer, bytesRead);
}

inline string readBytes(const int socket, const size_t bufferSize) {
    string info;
    info.resize(bufferSize);  
    size_t totalBytesRead = 0;
    while (totalBytesRead < bufferSize) {
        const ssize_t bytesRead = recv(socket, &info[totalBytesRead], bufferSize - totalBytesRead, 0);
        if (bytesRead < 0) {
            logger.error("Error in readBytes: " + string(strerror(errno)));
            return "";  
        }
        if (bytesRead == 0) break;  
        totalBytesRead += bytesRead;
    }
    info.resize(totalBytesRead);
    return info;
}

inline bool sendBytes(const int socket, const string& buffer) {
    size_t totalBytesSent = 0;
    const size_t bufferSize = buffer.size();
    while (totalBytesSent < bufferSize) {
        const ssize_t bytesToSend = min(bufferSize - totalBytesSent, BUFFER_SIZE);
        const ssize_t bytesSent = send(socket, &buffer[totalBytesSent], bytesToSend, 0);
        if (bytesSent < 0) {
            logger.error("Error in sendBytes: " + string(strerror(errno)));
            return false;
        }
        totalBytesSent += bytesSent;
    }
    return true;
}

inline void sendAcknowledge(const int socket) {
    const string ack = "ACK";
    send(socket, ack.c_str(), ack.size(), 0);
}

inline bool receiveAcknowledge(const int socket) {
    const string ack = "ACK";
    return readBytes(socket, ack.size()) == ack;
}

inline string toLower(string s) {
    ranges::transform(s, s.begin(), [](const unsigned char c) { return tolower(c); });
    return s;
}

inline vector<string> split(const string& input, const char delimiter) {
    vector<string> result;
    stringstream ss(input);
    string item;
    while (getline(ss, item, delimiter)) result.push_back(item);
    return result;
}

inline void printFileInfoTable(const string& data) {
    istringstream ss(data);
    string token;
    cout << left << setw(20) << "File Name" << setw(10) << "Size" << endl;
    cout << "---------------------------------------" << endl;
    while (getline(ss, token, ' ')) {
        string fileName;
        string fileSize;
        istringstream pairStream(token);
        getline(pairStream, fileName, ',');
        getline(pairStream, fileSize, ',');
        cout << left << setw(20) << fileName << setw(10) << fileSize << endl;
    }
}
