#ifndef FILE_INDEX_HEADER
#define FILE_INDEX_HEADER

#include "FileInfo.h"
#include "PeerFileInfo.h"
#include "NewPeerDTO.h"
#include <iostream>
#include <map>
#include <memory>

template<typename T>
class FileIndex {

private:
    map<FileInfo<T>, shared_ptr<FileInfo<T>>> info;
public:
    FileIndex() {}

    vector<pair<string, FileInfo<T>*>> find(string alias){
        vector<pair<string, FileInfo<T>*>> rs;
        for (auto &p : info) {
            string match = p.second->findMatch(alias);
            if(match!="") rs.emplace_back(match, p.second.get());
        }
        return rs;
    } 

    void addPeer(NewPeerDTO<T> peer) {
        for (auto &pfs : peer.peerFiles) {
            FileInfo<T> fileInfo(pfs.hash1, pfs.hash2, pfs.size);
            PeerFileInfo peerFileInfo{peer.ip, pfs.fileName, peer.port};
            auto it = info.find(fileInfo);
            if (it!=info.end()) {
                it->second->knownAs(peerFileInfo);
                continue;
            }
            info[fileInfo] = make_shared<FileInfo<T>>(pfs.hash1, pfs.hash2, pfs.size, peerFileInfo);
        }
    }
};

#endif 
