#ifndef PEER_INFO_HEADER
#define PEER_INFO_HEADER

#include <iostream>
#include <netinet/in.h>

struct PeerInfo {
    sockaddr_in6 ip;
    int port;
};

#endif
