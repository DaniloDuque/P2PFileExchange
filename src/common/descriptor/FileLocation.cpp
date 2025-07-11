#pragma once
#include <util.h>
#include <PeerDescriptor.cpp>

struct FileLocation {
    const PeerDescriptor peer;
    const string filename;

    FileLocation() = delete;

    FileLocation(const PeerDescriptor &peer, const string &filename) : peer(peer), filename(filename) {
    }

    bool operator<(const FileLocation &other) const {
        if (peer != other.peer) return peer < other.peer;
        return filename < other.filename;
    }

    bool operator==(const FileLocation &other) const {
        return peer == other.peer && filename == other.filename;
    }

    string serialize() const {
        return filename + "," + peer.serialize();
    }

    static FileLocation deserialize(const string &data) {
        istringstream ss(data);
        string filename, ip, port_str;
        getline(ss, filename, ',');
        getline(ss, ip, ',');
        getline(ss, port_str, ',');
        const int port = stoi(port_str);
        return FileLocation(PeerDescriptor(ip, port), filename);
    }
};
