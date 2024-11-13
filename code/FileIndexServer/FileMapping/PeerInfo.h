#ifndef PEER_INFO_HEADER
#define PEER_INFO_HEADER

#include "../../util.h"
#include "Trie.h"
#include "PeerInfoDTO.h"

template<typename T>
struct PeerInfo {

    string ip;
    int port;
    Trie<T> files;

    PeerInfoDTO<T> findFiles(string name) {
        return PeerInfoDTO<T>{ip, port, files.search(name)};
    }

};

#endif
