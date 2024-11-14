#ifndef NEW_PEER_DTO
#define NEW_PEER_DTO

#include "../util.h"

template<typename T>
struct NewPeerDTO {
    string ip;
    int port;
    vector<File<T>> peerFiles;

    string serialize(){

    }

    static NewPeerDTO<T> deserialize(string &data){

    }
}

#endif
