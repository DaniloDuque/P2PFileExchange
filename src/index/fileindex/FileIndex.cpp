#pragma once
#include "index/file/FileInfo.cpp"
#include "dto/NewPeerDTO.cpp"
#include "dto/PeerFileInfoDTO.cpp"
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <shared_mutex>

class FileIndex {
    
    struct Key {
        ll hash1, hash2, size;
    
        bool operator<(const Key& other) const {
            if (hash1 != other.hash1) return hash1 < other.hash1;
            if (hash2 != other.hash2) return hash2 < other.hash2;
            return size < other.size;
        }
    
        bool operator==(const Key& other) const {
            return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
        }
    };
    
    map<Key, shared_ptr<FileInfo>> info;
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

    shared_ptr<FileInfo> find(const ll hash1, const ll hash2, const ll size) const {
        shared_lock lock(indexMutex);
        const Key key{hash1, hash2, size};
        if (const auto it = info.find(key); it != info.end()) {
            return it->second;
        }
        return nullptr;
    }

    void addPeer(const NewPeerDTO& peer) {
        unique_lock lock(indexMutex);
        for (auto &pfs : peer.peerFiles) {
            Key key{pfs.hash1, pfs.hash2, pfs.size};
            PeerFileInfoDTO peerFileInfo{peer.ip, pfs.filename, peer.port};
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
            if (it->second->isEmpty()) it = info.erase(it);
            else ++it;
        }
    }
};

