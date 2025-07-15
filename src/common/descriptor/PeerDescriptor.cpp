#pragma once
#include <util.h>

struct PeerDescriptor {
    const string ip{};
    const int port;

    PeerDescriptor() = delete;

    PeerDescriptor(const string &ip, const int port) : ip(ip), port(port) {
    }

    bool operator<(const PeerDescriptor &other) const {
        if (port != other.port) return port < other.port;
        return ip < other.ip;
    }

    bool operator==(const PeerDescriptor &other) const {
        return port == other.port && ip == other.ip;
    }

    string serialize() const {
        return ip + ',' + to_string(port);
    }

    static PeerDescriptor deserialize(const string &data) {
        if (data.empty()) {
            throw invalid_argument("Empty PeerDescriptor data");
        }

        istringstream ss(data);
        string ip, token;

        if (!getline(ss, ip, ',') || ip.empty() || ip.length() > 15) {
            throw invalid_argument("Invalid IP address in PeerDescriptor");
        }

        if (!getline(ss, token, ',') || token.empty()) {
            throw invalid_argument("Missing port in PeerDescriptor");
        }

        int port;
        try {
            port = stoi(token);
        } catch (const exception &) {
            throw invalid_argument("Invalid port number in PeerDescriptor");
        }

        if (port <= 0 || port > 65535) {
            throw invalid_argument("Port out of range: " + to_string(port));
        }

        return PeerDescriptor(ip, port);
    }
};
