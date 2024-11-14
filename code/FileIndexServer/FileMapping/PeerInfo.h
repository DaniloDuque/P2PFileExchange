#ifndef PEER_INFO_HEADER
#define PEER_INFO_HEADER

#include "../../util.h"
#include "Trie.h"
#include "PeerInfoDTO.h"
#include "../NewPeerDTO.h"

template<typename T>
struct PeerInfo {

    string ip;
    int port;
    Trie<T> files = new Trie<T>();

    PeerInfo(NewPeerDTO<T> dto){
        ip = dto.ip;
        port = dto.port;
        for(auto &f : dto.peerFiles) files.insert(f);
    }

    PeerInfoDTO<T> findFiles(string name) {
        return PeerInfoDTO<T>{ip, port, files.search(name)};
    }

};

#endif
