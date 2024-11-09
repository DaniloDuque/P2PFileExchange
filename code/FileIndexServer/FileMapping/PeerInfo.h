#ifndef PEER_INFO_HEADER
#define PEER_INFO_HEADER

#include <iostream>
#include <netinet/in.h>
#include "../../util.h"

struct PeerInfo {

    string ip;
    int port;

    string serialize() const {
        return ip + ',' + to_string(port);  
    }

    static PeerInfo deserialize(const string& data) {
        PeerInfo peerInfo;
        istringstream ss(data);
        string ip_str;
        getline(ss, ip_str, ',');
        string port_str;
        getline(ss, port_str);
        peerInfo.ip = ip_str;
        peerInfo.port = stoi(port_str);
        return peerInfo;
    }

    bool operator<(const PeerInfo& other) const {
        if(ip == other.ip) return port < other.port;
        return ip < other.ip;
    }

    bool operator==(const PeerInfo& other) const {
        return ip == other.ip && port == other.port;   
    }
};

#endif
