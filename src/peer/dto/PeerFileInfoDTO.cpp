#pragma once
#include <iostream>
#include <netinet/in.h>
#include "../../util.h"
#include "../../logger/Logger.h"

struct PeerFileInfoDTO {

    string ip, fileName;
    int port;

    string serialize() const {
        return fileName + ',' + ip + ',' + to_string(port);  
    }

    static PeerFileInfoDTO deserialize(const string& data) {
        PeerFileInfoDTO peerFileInfo;
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

    bool operator<(const PeerFileInfoDTO& other) const {
        if (ip != other.ip) return ip < other.ip;
        if (port != other.port) return port < other.port;
        return fileName < other.fileName;
    }

    bool operator==(const PeerFileInfoDTO& other) const {
        return ip == other.ip && port == other.port && fileName == other.fileName;
    }

};
