#ifndef PEER_FILE_DTO_HEADER
#define PEER_FILE_DTO_HEADER

#include "../util.h"
#include "FileMapping/PeerInfo.h"

template<typename T>
struct PeerFileDTO {
    T h1, h2, size;
    string alias;
    PeerInfo info;
};

#endif
