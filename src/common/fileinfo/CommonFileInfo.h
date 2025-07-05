#pragma once
#include <iostream>
#include "dto/PeerFileInfoDTO.cpp"
using namespace std;

template<typename Derived>
class CommonFileInfo {
protected:
    const ll hash1, hash2, size;
    set<PeerFileInfoDTO> fileInfo; 

public:
    void knownAs(const PeerFileInfoDTO& info) {
        fileInfo.insert(info);
    }

    CommonFileInfo(const ll h1, const ll h2, const ll sz) : hash1(h1), hash2(h2), size(sz) {};
    CommonFileInfo(const ll h1, const ll h2, const ll sz, const PeerFileInfoDTO &info) : hash1(h1), hash2(h2), size(sz) {
        knownAs(info);
    };

    ll getHash1() const { return hash1; }
    ll getHash2() const { return hash2; }
    ll getSize() const { return size; }

    virtual string findMatch(string) = 0;
    virtual ~CommonFileInfo() = default;

    bool operator==(const CommonFileInfo& other) const {
        return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
    }

    bool operator<(const CommonFileInfo& other) const {
        if (hash1 != other.hash1) return hash1 < other.hash1;
        if (hash2 != other.hash2) return hash2 < other.hash2;
        return size < other.size;
    }

    string serialize() const {
        string rsp = to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size); 
        for(auto &pf : fileInfo) rsp += " " + pf.serialize();
        return rsp;
    }       

    static Derived deserialize(const string& data) {
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        ll h1 = stoll(token); 
        getline(ss, token, ',');
        ll h2 = stoll(token); 
        getline(ss, token, ' '); 
        ll sz = stoll(token); 
        Derived newFileInfo(h1, h2, sz);
        while (getline(ss, token, ' ')) {
            PeerFileInfoDTO peerFileInfo = PeerFileInfoDTO::deserialize(token);
            newFileInfo.knownAs(peerFileInfo);
        }
        return newFileInfo;
    }

};