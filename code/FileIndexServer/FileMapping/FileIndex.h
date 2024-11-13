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
    FileIndex();
    vector<PeerInfoDTO<T>*> find(string); 
    //void update(PeerFileDTO<T>);
};


template<typename T>
FileIndex<T>::FileIndex() {

}

// template<typename T>
// void FileIndex<T>::update(PeerFileDTO<T> dto){
//     FileInfo<T> cur(dto.h1, dto.h2, dto.size);
//     auto it = info.find(cur);
//     if(it!=info.end()){
//         it->second->knownAs(dto.alias, dto.info);
//         return;
//     }
//     info[cur]=make_shared<FileInfo<T>>(dto.h1,dto.h2,dto.size,dto.alias,dto.info);
// } 
//

template<typename T>
vector<PeerInfoDTO<T>*> FileIndex<T>::find(string alias) {
    vector<PeerInfoDTO<T>*> rs;  
    for (auto &p : peerShared)
        for(auto &i : p.findFiles(alias)) rs.push_back(i); 
    return rs; 
}

#endif 
