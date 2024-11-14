#ifndef NEW_PEER_DTO_HEADER
#define NEW_PEER_DTO_HEADER

#include "FileDTO.h"

template<typename T>
struct NewPeerDTO {
    string ip;
    int port;
    vector<FileDTO<T>> peerFiles;
};

#endif
