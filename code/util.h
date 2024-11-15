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
#include <iomanip>

#define BUFFER_SIZE (1<<18)
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
        ssize_t bytesToSend = min(bufferSize - totalBytesSent, static_cast<ll>(BUFFER_SIZE));
        ssize_t bytesSent = write(socket, &buffer[totalBytesSent], bytesToSend);
        if (bytesSent < 0) {
            cerr << "Error in sendBytes: " << strerror(errno) << endl;
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

vector<string> split(const string& input, char delimiter) {
    vector<string> result;
    stringstream ss(input);
    string item;
    while (getline(ss, item, delimiter)) result.push_back(item);
    return result;
}


void printFileInfoTable(const string& data) {
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
#endif
