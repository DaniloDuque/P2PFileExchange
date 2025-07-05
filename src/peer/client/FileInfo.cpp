#pragma once
#include <iostream>
#include <vector>
#include "../dto/PeerFileInfoDTO.cpp"
using namespace std;

class FileInfo {
private:
    const ll hash1, hash2, size;
    set<PeerFileInfoDTO> fileInfo; 

public:
    FileInfo(ll h1, ll h2, ll sz, PeerFileInfoDTO info) 
        : hash1(h1), hash2(h2), size(sz) {
        knownAs(info);  
    }    

    FileInfo(ll h1, ll h2, ll sz) : hash1(h1), hash2(h2), size(sz) {} 

    ll getSize() const { return size; }
    ll getHash1() const { return hash1; }
    ll getHash2() const { return hash2; }

    set<PeerFileInfoDTO> getFileInfo() { return fileInfo; }
    ll getNumberOfPeersWithFile() const { return fileInfo.size(); }

    void knownAs(PeerFileInfoDTO info) { fileInfo.insert(info); }

    string findMatch(string alias){
        for(auto &pfi : fileInfo) if(pfi.fileName == alias) return pfi.fileName;
        return "";
    }     

    string serialize() const {
        string rsp = to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size); 
        for(auto &pf : fileInfo) rsp += " " + pf.serialize();
        return rsp;
    }    

    static FileInfo deserialize(const string& data) {
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        ll h1 = (stoll(token)); 
        getline(ss, token, ',');
        ll h2 = (stoll(token)); 
        getline(ss, token, ' '); 
        ll sz = (stoll(token)); 
        FileInfo newFileInfo(h1, h2, sz);
        while (getline(ss, token, ' ')) {
            PeerFileInfoDTO peerFileInfo = PeerFileInfoDTO::deserialize(token);
            newFileInfo.knownAs(peerFileInfo);
        }
        return newFileInfo;
    }    

    bool operator == (const FileInfo& other) const {
        return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
    }

    bool operator<(const FileInfo& other) const {
        if (hash1 != other.hash1) return hash1 < other.hash1;
        if (hash2 != other.hash2) return hash2 < other.hash2;
        return size < other.size;
    }

};
