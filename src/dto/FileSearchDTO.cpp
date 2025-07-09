#pragma once
#include "util.h"
#include "common/descriptor/PeerDescriptor.cpp"

struct FileSearchDTO {
    PeerDescriptor peer;
    string filename;

    FileSearchDTO() = default;
    FileSearchDTO(const PeerDescriptor& peer, const string& filename) : peer(peer), filename(filename) {}

    string serialize() const {
        return filename + ',' + peer.serialize();
    }

    static FileSearchDTO deserialize(const string &data) {
        istringstream ss(data);
        string filename, ip, token;

        getline(ss, filename, ',');
        getline(ss, ip, ',');
        getline(ss, token, ',');
        const int port = stoi(token);

        return FileSearchDTO(PeerDescriptor(ip, port), filename);
    }

};