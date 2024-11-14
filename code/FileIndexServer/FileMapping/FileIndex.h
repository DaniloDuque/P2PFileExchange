#ifndef FILE_INDEX_HEADER
#define FILE_INDEX_HEADER

#include <iostream>
#include <map>
#include <memory>
#include "PeerInfo.h"
#include "PeerInfoDTO.h"

template<typename T>
class FileIndex {

private:
    vector<PeerInfo<T>> peerShared;

public:
    FileIndex() {}

    FileInfoDTO<T> find(int idx, string &name){

    }

    vector<PeerInfoDTO<T>*> find(string alias) {
        vector<PeerInfoDTO<T>*> rs;  
        for (auto &p : peerShared)
            for(auto &i : p.findFiles(alias)) rs.push_back(i); 
        return rs; 
    }

    void addPeer(NewPeerDTO<T> dto){
        peerShared.push_back(PeerInfo<T>(dto));
    }
};

#endif 
