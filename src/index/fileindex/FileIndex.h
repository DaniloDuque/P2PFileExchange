#pragma once
#include <index/file/FileInfo.cpp>
#include <common/descriptor/FileDescriptor.cpp>
#include <common/descriptor/SearchResult.cpp>
#include <dto/AddPeerDTO.cpp>
#include <map>
#include <memory>
#include <ranges>
#include <shared_mutex>

class FileIndex {
    map<FileDescriptor, shared_ptr<FileInfo> > info;
    mutable shared_mutex indexMutex;

public:
    SearchResult find(const string &alias) const;

    shared_ptr<FileInfo> find(const FileDescriptor &descriptor) const;

    void add_peer(const AddPeerDTO &peer);

    void remove_peer(const PeerDescriptor &peer);
};
