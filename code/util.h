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

#define INDEX_PORT 8080
#define INDEX_IP "6.tcp.ngrok.k"
#define PEER_PORT 8080
#define BUFFER_SIZE (1<<13)
#define ll long long
#define uchar unsigned char

using namespace std;

string sockaddr_in6_to_string(const sockaddr_in6& addr) {
    char str[INET6_ADDRSTRLEN]; 
    if (inet_ntop(AF_INET6, &addr.sin6_addr, str, sizeof(str)) == nullptr) {
        return "Error en conversión"; 
    }
    return string(str); 
}

#endif
