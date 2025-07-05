#pragma once
#include "index/file/FileInfo.cpp"
#include "dto/NewPeerDTO.cpp"
#include "dto/PeerFileInfoDTO.cpp"
#include <iostream>
#include <map>
#include <memory>
#include <ranges>

class FileIndex {
    map<FileInfo, shared_ptr<FileInfo>> info;
public:
    FileIndex() {}

    vector<pair<string, FileInfo*>> find(const string& alias) const {
        vector<pair<string, FileInfo*>> rs;
        for (const auto &val: info | views::values) {
            if(string match = val->findMatch(alias); !match.empty()) rs.emplace_back(match, val.get());
        }
        return rs;
    } 

    void addPeer(const NewPeerDTO& peer) {
        for (auto &pfs : peer.peerFiles) {
            FileInfo fileInfo(pfs.hash1, pfs.hash2, pfs.size);
            PeerFileInfoDTO peerFileInfo{peer.ip, pfs.fileName, peer.port};
            if (auto it = info.find(fileInfo); it!=info.end()) {
                it->second->knownAs(peerFileInfo);
                continue;
            }
            info[fileInfo] = make_shared<FileInfo>(pfs.hash1, pfs.hash2, pfs.size, peerFileInfo);
        }
    }
    
    void removePeer(const string& ip, const int port) {
        for (auto it = info.begin(); it != info.end();) {
            it->second->removePeer(ip, port);
            if (it->second->isEmpty()) {
                it = info.erase(it);
            } else {
                ++it;
            }
        }
    }
};
