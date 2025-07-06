#pragma once
#include "index/file/FileInfo.cpp"
#include "dto/NewPeerDTO.cpp"
#include "dto/PeerFileInfoDTO.cpp"
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <shared_mutex>

struct FileKey {
    ll hash1, hash2, size;
    
    bool operator<(const FileKey& other) const {
        if (hash1 != other.hash1) return hash1 < other.hash1;
        if (hash2 != other.hash2) return hash2 < other.hash2;
        return size < other.size;
    }
    
    bool operator==(const FileKey& other) const {
        return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
    }
};

class FileIndex {
    map<FileKey, shared_ptr<FileInfo>> info;
    mutable shared_mutex indexMutex;

public:
    vector<pair<string, shared_ptr<FileInfo>>> find(const string& alias) const {
        shared_lock lock(indexMutex);
        vector<pair<string, shared_ptr<FileInfo>>> rs;
        for (const auto &val: info | views::values) {
            if(string match = val->findMatch(alias); !match.empty()) rs.emplace_back(match, val);
        }
        return rs;
    }

    void addPeer(const NewPeerDTO& peer) {
        unique_lock lock(indexMutex);
        for (auto &pfs : peer.peerFiles) {
            FileKey key{pfs.hash1, pfs.hash2, pfs.size};
            PeerFileInfoDTO peerFileInfo{peer.ip, pfs.fileName, peer.port};
            if (auto it = info.find(key); it!=info.end()) {
                it->second->knownAs(peerFileInfo);
                continue;
            }
            info[key] = make_shared<FileInfo>(pfs.hash1, pfs.hash2, pfs.size, peerFileInfo);
        }
    }

    void removePeer(const string& ip, const int port) {
        unique_lock lock(indexMutex);
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

