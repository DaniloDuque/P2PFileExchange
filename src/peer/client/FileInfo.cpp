#pragma once
#include <iostream>
#include "dto/PeerFileInfoDTO.cpp"
#include "common/fileinfo/CommonFileInfo.h"
using namespace std;

class FileInfo final : public CommonFileInfo<FileInfo> {
public:
    FileInfo(const ll h1, const ll h2, const ll sz) : CommonFileInfo(h1, h2, sz) {}
    FileInfo(const ll h1, const ll h2, const ll sz, const PeerFileInfoDTO &info) : CommonFileInfo(h1, h2, sz, info) {}

    set<PeerFileInfoDTO> getFileInfo() { 
        shared_lock lock(infoMutex);
        return fileInfo; 
    }
    size_t getNumberOfPeersWithFile() const { 
        shared_lock lock(infoMutex);
        return fileInfo.size(); 
    }

    string findMatch(const string alias) override {
        shared_lock lock(infoMutex);
        for(auto &pfi : fileInfo) if(pfi.filename == alias) return pfi.filename;
        return "";
    }     

};
