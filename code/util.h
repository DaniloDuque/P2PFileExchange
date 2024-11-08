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

#define BUFFER_SIZE (1<<13)
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

string readBuffer(int socket, int bufferSize){
    char buffer[bufferSize] = {};
    read(socket, buffer, bufferSize);
    string info = buffer;
    return info;
}

void sendAcknowledge(int socket){
    string ack = "0";
    send(socket, ack.c_str(), ack.size(), 0);
}

string toLower(string s){
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return tolower(c);});
    return s;
}

#endif
