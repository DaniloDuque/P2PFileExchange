#pragma once

#include <iostream>
#include <thread>
#include <string>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <utility>
#include <fstream>
#include <vector>
#include <sys/socket.h>
#include <stdlib.h>
#include <cstring>
#include <stdarg.h>
#include <limits>
#include <cstdlib>
#include <netdb.h>
#include <sstream>
#include <algorithm> 
#include <cctype>
#include <set>
#include <mutex>
#include <iomanip>

#define BUFFER_SIZE (1<<10)
#define ll long long
#define uchar unsigned char
using namespace std;

inline string readSingleBuffer(int socket) {
    char buffer[BUFFER_SIZE] = {};
    ssize_t bytesRead = recv(socket, buffer, BUFFER_SIZE, 0);
    if (bytesRead < 0) {
        cerr << "Error in readSingleBuffer: " << strerror(errno) << endl;
        return "";
    }
    return string(buffer, bytesRead);
}

inline string readBytes(int socket, int bufferSize) {
    string info;
    info.resize(bufferSize);  
    ll totalBytesRead = 0;
    while (totalBytesRead < bufferSize) {
        ssize_t bytesRead = recv(socket, &info[totalBytesRead], bufferSize - totalBytesRead, 0);
        if (bytesRead < 0) {
            cerr << "Error in readBytes: " << strerror(errno) << endl;
            return "";  
        }
        if (bytesRead == 0) break;  
        totalBytesRead += bytesRead;
    }
    info.resize(totalBytesRead);
    return info;
}

inline bool sendBytes(int socket, string& buffer) {
    ll totalBytesSent = 0;
    ll bufferSize = buffer.size();
    while (totalBytesSent < bufferSize) {
        ssize_t bytesToSend = min(bufferSize - totalBytesSent, static_cast<ll>(BUFFER_SIZE));
        ssize_t bytesSent = send(socket, &buffer[totalBytesSent], bytesToSend, 0);
        if (bytesSent < 0) {
            cerr << "Error in sendBytes: " << strerror(errno) << endl;
            return false;
        }
        totalBytesSent += bytesSent;
    }
    return true;
}

inline void sendAcknowledge(int socket) {
    const string ack = "ACK";
    send(socket, ack.c_str(), ack.size(), 0);
}

inline bool receiveAcknowledge(int socket) {
    const string ack = "ACK";
    return readBytes(socket, ack.size()) == ack;
}

inline string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return tolower(c); });
    return s;
}

inline vector<string> split(const string& input, char delimiter) {
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
