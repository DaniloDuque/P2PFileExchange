#ifndef PEER_INFO_DTO_HEADER
#define PEER_INFO_DTO_HEADER

#include "../../util.h"

template<typename T>
struct PeerInfoDTO {
    
    string ip;
    int port;
    vector<File<T>*> files;
    PeerInfoDTO(string addr, int p, vector<File<T>*> f): ip(addr), port(p), files(f) {}

};

#endif
