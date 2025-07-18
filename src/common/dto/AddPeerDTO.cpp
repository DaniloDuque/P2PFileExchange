#pragma once
#include <util.h>
#include <common/descriptor/PeerDescriptor.cpp>
#include <common/descriptor/IndexedFileDescriptor.cpp>

struct AddPeerDTO {
    PeerDescriptor peer;
    set<IndexedFileDescriptor> indexed_files;

    AddPeerDTO() = delete;

    AddPeerDTO(const PeerDescriptor &peer,
               const set<IndexedFileDescriptor> &indexed_files) : peer(peer), indexed_files(indexed_files) {
    }

    string serialize() const {
        string ser = peer.ip + ',' + to_string(peer.port);
        for (auto &p: indexed_files) ser += " " + p.serialize();
        return ser;
    }

    static AddPeerDTO deserialize(const string &data) {
        if (data.empty()) {
            throw invalid_argument("Empty AddPeerDTO data");
        }

        istringstream ss(data);
        string token;

        if (!getline(ss, token, ' ') || token.empty()) {
            throw invalid_argument("Invalid peer descriptor in AddPeerDTO");
        }

        const PeerDescriptor peer = PeerDescriptor::deserialize(token);

        set<IndexedFileDescriptor> indexed_files;
        while (ss >> token) {
            if (!token.empty()) {
                indexed_files.insert(IndexedFileDescriptor::deserialize(token));
            }
        }

        return AddPeerDTO(peer, indexed_files);
    }
};
