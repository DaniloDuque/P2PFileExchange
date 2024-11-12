#ifndef UTIL_HEADER
#define UTIL_HEADER

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

#define BUFFER_SIZE (1<<10)
#define ll long long
#define uchar unsigned char

using namespace std;

string sockaddr_in6_to_string(const sockaddr_in6& addr) {
    char str[INET6_ADDRSTRLEN]; 
    if (inet_ntop(AF_INET6, &addr.sin6_addr, str, sizeof(str)) == nullptr) {
        return "Error in IP conversion"; 
    }
    return string(str); 
}

string readSingleBuffer(int socket){
    char buffer[BUFFER_SIZE] = {};
    read(socket, buffer, BUFFER_SIZE);
    string info = buffer;
    return info;
}

string readBytes(int socket, int bufferSize) {
    string info;
    info.resize(bufferSize);  
    ll totalBytesRead = 0;
    while (totalBytesRead < bufferSize) {
        ssize_t bytesRead = read(socket, &info[totalBytesRead], bufferSize - totalBytesRead);
        if (bytesRead < 0) {
            cerr << "Error in readBuffer: " << strerror(errno) << endl;
            return "";  
        }
        if (bytesRead == 0) break;
        totalBytesRead += bytesRead;
    }
    info.resize(totalBytesRead);
    return info;
}

void sendBytes(int socket, string& buffer) {
    ll totalBytesSent = 0;
    ll bufferSize = buffer.size();
    while (totalBytesSent < bufferSize) {
        ssize_t bytesToSend = std::min(bufferSize - totalBytesSent, static_cast<ll>(BUFFER_SIZE));
        ssize_t bytesSent = write(socket, &buffer[totalBytesSent], bytesToSend);
        if (bytesSent < 0) {
            std::cerr << "Error in sendBytes: " << strerror(errno) << std::endl;
            return;  
        }
        totalBytesSent += bytesSent;
    }
}

void sendAcknowledge(int socket){
    string ack = "0";
    send(socket, ack.c_str(), ack.size(), 0);
}

string receiveAcknowledge(int socket){
    return readBytes(socket, 1);
}

string toLower(string s){
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return tolower(c);});
    return s;
}

#endif
