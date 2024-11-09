


#ifndef PEER_INFO_HEADER

#define PEER_INFO_HEADER
#include "../util.h"


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
};



#endif
