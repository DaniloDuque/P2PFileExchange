#pragma once
#include "index/file/FileInfo.cpp"
#include "common/descriptor/FileDescriptor.cpp"
#include "common/descriptor/FileLocation.cpp"
#include "common/descriptor/SearchResult.cpp"
#include "dto/AddPeerDTO.cpp"
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <shared_mutex>

class FileIndex {
    map<FileDescriptor, shared_ptr<FileInfo>> info;
    mutable shared_mutex indexMutex;

public:
    SearchResult find(const string& alias) const {
        shared_lock lock(indexMutex);
        SearchResult rs;
        for (const auto &val: info | views::values) {
            if(string match = val->findMatch(alias); !match.empty())
                rs.values.emplace_back(match, val->getFileDescriptor());
        }
        return rs;
    }

    shared_ptr<FileInfo> find(const FileDescriptor& descriptor) const {
        shared_lock lock(indexMutex);
        if (const auto it = info.find(descriptor); it != info.end()) {
            return it->second;
        }
        return nullptr;
    }

    void addPeer(const AddPeerDTO& peer) {
        unique_lock lock(indexMutex);
        for (auto &pfs : peer.indexed_files) {
            auto key = pfs.file;
            auto value = FileLocation(peer.peer, pfs.filename);
            if (auto it = info.find(key); it!=info.end()) {
                it->second->knownAs(value);
                continue;
            }
            info[key] = make_shared<FileInfo>(key, value);
        }
    }

    void removePeer(const PeerDescriptor& peer) {
        unique_lock lock(indexMutex);
        for (auto it = info.begin(); it != info.end();) {
            it->second->removePeer(peer);
            if (it->second->isEmpty()) it = info.erase(it);
            else ++it;
        }
    }
};

