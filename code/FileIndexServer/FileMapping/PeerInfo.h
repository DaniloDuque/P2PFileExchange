#include <iostream>
#include <netinet/in.h>

struct PeerInfo {
    sockaddr_in6 ip;
    int port;
};
