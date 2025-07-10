#pragma once
#include "util.h"

struct PeerDescriptor {
    const string ip{};
    const int port;

    PeerDescriptor() = delete;
    PeerDescriptor(const string& ip, const int port) : ip(ip), port(port) {}

    bool operator<(const PeerDescriptor& other) const {
        if (port != other.port) return port < other.port;
        return ip < other.ip;
    }

    bool operator==(const PeerDescriptor& other) const {
        return port == other.port && ip == other.ip;
    }

    string serialize() const {
        return ip + ',' + to_string(port);
    }

    static PeerDescriptor deserialize(const string &data) {
        istringstream ss(data);
        string ip, token;
        getline(ss, ip, ',');
        getline(ss, token, ',');
        const int port = stoi(token);
        return PeerDescriptor(ip, port);
    }

};
