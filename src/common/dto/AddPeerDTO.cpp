#pragma once
#include "util.h"
#include "common/descriptor/PeerDescriptor.cpp"
#include "common/descriptor/IndexedFileDescriptor.cpp"

struct AddPeerDTO {
    PeerDescriptor peer;
    vector<IndexedFileDescriptor> indexed_files;

    AddPeerDTO() = delete;
    AddPeerDTO(const PeerDescriptor& peer, const vector<IndexedFileDescriptor>& indexed_files) : peer(peer), indexed_files(indexed_files) {}

    string serialize() const {
        string ser = peer.ip + ',' + to_string(peer.port);
        for(auto &p : indexed_files) ser += " " + p.serialize();
        return ser;
    }

    static AddPeerDTO deserialize(const string& data) {
        istringstream ss(data);
        string token;

        getline(ss, token, ' ');
        const PeerDescriptor peer = PeerDescriptor::deserialize(token);

        vector<IndexedFileDescriptor> indexed_files;
        while (getline(ss, token, ' ')) {
            if (!token.empty())
                indexed_files.push_back(IndexedFileDescriptor::deserialize(token));
        }

        return AddPeerDTO(peer, indexed_files);
    }

};