#pragma once
#include "../file/FileInfo.cpp"
#include "../../dto/NewPeerDTO.cpp"
#include "../../dto/PeerFileInfoDTO.cpp"
#include <iostream>
#include <map>
#include <memory>

class FileIndex {
private:
    map<FileInfo, shared_ptr<FileInfo>> info;
public:
    FileIndex() {}

    vector<pair<string, FileInfo*>> find(string alias){
        vector<pair<string, FileInfo*>> rs;
        for (auto &p : info) {
            string match = p.second->findMatch(alias);
            if(match != "") rs.emplace_back(match, p.second.get());
        }
        return rs;
    } 

    void addPeer(NewPeerDTO peer) {
        for (auto &pfs : peer.peerFiles) {
            FileInfo fileInfo(pfs.hash1, pfs.hash2, pfs.size);
            PeerFileInfoDTO peerFileInfo{peer.ip, pfs.fileName, peer.port};
            auto it = info.find(fileInfo);
            if (it!=info.end()) {
                it->second->knownAs(peerFileInfo);
                continue;
            }
            info[fileInfo] = make_shared<FileInfo>(pfs.hash1, pfs.hash2, pfs.size, peerFileInfo);
        }
    }
};
