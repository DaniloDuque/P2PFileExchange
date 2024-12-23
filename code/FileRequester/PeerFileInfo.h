#ifndef PEER_INFO_HEADER
#define PEER_INFO_HEADER

#include <iostream>
#include <netinet/in.h>
#include "../util.h"

struct PeerFileInfo {

    string ip, fileName;
    int port;

    string serialize() const {
        return fileName + ',' + ip + ',' + to_string(port);  
    }

    static PeerFileInfo deserialize(const string& data) {
        PeerFileInfo peerFileInfo;
        istringstream ss(data);
        string fileName;
        getline(ss, fileName, ',');
        string ip_str;
        getline(ss, ip_str, ',');
        string port_str;
        getline(ss, port_str);
        peerFileInfo.fileName = fileName;
        peerFileInfo.ip = ip_str;
        peerFileInfo.port = stoi(port_str);
        return peerFileInfo;
    }

    bool operator<(const PeerFileInfo& other) const {
        if (ip != other.ip) return ip < other.ip;
        if (port != other.port) return port < other.port;
        return fileName < other.fileName;
    }

    bool operator==(const PeerFileInfo& other) const {
        return ip == other.ip && port == other.port && fileName == other.fileName;
    }

};

#endif
