#pragma once
#include <iostream>
#include <netinet/in.h>
#include "../../util.h"

struct PeerFileInfo {

    string ip, filename;
    int port;

    string serialize() const {
        return filename + ',' + ip + ',' + to_string(port);  
    }

    static PeerFileInfo deserialize(const string& data) {
        PeerFileInfo peerFileInfo;
        istringstream ss(data);
        string filename;
        getline(ss, filename, ',');
        string ip_str;
        getline(ss, ip_str, ',');
        string port_str;
        getline(ss, port_str);
        peerFileInfo.filename = filename;
        peerFileInfo.ip = ip_str;
        peerFileInfo.port = stoi(port_str);
        return peerFileInfo;
    }

    bool operator<(const PeerFileInfo& other) const {
        if (ip != other.ip) return ip < other.ip;
        if (port != other.port) return port < other.port;
        return filename < other.filename;
    }

    bool operator==(const PeerFileInfo& other) const {
        return ip == other.ip && port == other.port && filename == other.filename;
    }

};