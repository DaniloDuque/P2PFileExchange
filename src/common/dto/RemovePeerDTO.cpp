#pragma once
#include "common/descriptor/PeerDescriptor.cpp"

struct RemovePeerDTO {
    const PeerDescriptor peer;

    RemovePeerDTO() = delete;
    RemovePeerDTO(const PeerDescriptor& peer) : peer(peer) {}

    string serialize() const { return peer.serialize(); }

    static RemovePeerDTO deserialize(const string& data) {
        return RemovePeerDTO(PeerDescriptor::deserialize(data));
    }

};
