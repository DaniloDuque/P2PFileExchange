#pragma once
#include <iostream>
#include <vector>
#include "../../dto/PeerFileInfoDTO.cpp"
#include "../../common/fileinfo/CommonFileInfo.h"
using namespace std;

class FileInfo : public CommonFileInfo<FileInfo> {
public:
    FileInfo(ll h1, ll h2, ll sz) : CommonFileInfo<FileInfo>(h1, h2, sz) {} 
    FileInfo(ll h1, ll h2, ll sz, PeerFileInfoDTO info) : CommonFileInfo<FileInfo>(h1, h2, sz, info) {}

    set<PeerFileInfoDTO> getFileInfo() { return fileInfo; }
    ll getNumberOfPeersWithFile() const { return fileInfo.size(); }

    string findMatch(string alias) override {
        for(auto &pfi : fileInfo) if(pfi.filename == alias) return pfi.filename;
        return "";
    }     

};
