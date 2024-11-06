#ifndef PEER_INFO_HEADER
#define PEER_INFO_HEADER

#include <iostream>
#include <netinet/in.h>
#include "../../util.h"

struct PeerInfo {
    sockaddr_in6 ip;
    int port;

    string serialize() const {
        return sockaddr_in6_to_string(ip) + ',' + to_string(port);  
    }

    static PeerInfo deserialize(const string& data) {
        PeerInfo peerInfo;
        istringstream ss(data);
        string ip_str;
        getline(ss, ip_str, ',');
        string port_str;
        getline(ss, port_str);
        inet_pton(AF_INET6, ip_str.c_str(), &peerInfo.ip.sin6_addr);
        peerInfo.ip.sin6_family = AF_INET6;
        peerInfo.port = stoi(port_str);
        return peerInfo;
    }

    bool operator<(const PeerInfo& other) const {
        int cmp = memcmp(&ip.sin6_addr, &other.ip.sin6_addr, sizeof(in6_addr));
        if (cmp != 0) {
            return cmp < 0;
        }
        return port < other.port;
    }

    bool operator==(const PeerInfo& other) const {
        return memcmp(&ip.sin6_addr, &other.ip.sin6_addr, sizeof(in6_addr)) == 0 && port == other.port;
    }

};

#endif
