#ifndef FILEINFO_H
#define FILEINFO_H

#include <iostream>
#include <vector>
#include "PeerFileInfo.h"
using namespace std;

template<typename T>
class FileInfo {
private:
    const T hash1, hash2, size;
    set<PeerFileInfo> fileInfo; 

public:
    FileInfo(T, T, T, PeerFileInfo);
    FileInfo(T, T, T);
    T getSize() const;
    T getHash1() const;
    T getHash2() const;
    set<PeerFileInfo> getFileInfo();
    ll getNumberOfPeersWithFile();
    void knownAs(PeerFileInfo);
    string findMatch(string);
    string serialize() const;         
    static FileInfo<T> deserialize(const string&);
    bool operator == (const FileInfo&) const;
    bool operator<(const FileInfo&) const;
};

template<typename T>
FileInfo<T>::FileInfo(T h1, T h2, T sz) : hash1(h1), hash2(h2), size(sz) {}

template<typename T>
FileInfo<T>::FileInfo(T h1, T h2, T sz, PeerFileInfo info) 
    : hash1(h1), hash2(h2), size(sz) {
    knownAs(info);  
}

template<typename T>
void FileInfo<T>::knownAs(PeerFileInfo info) {
    fileInfo.insert(info);
}

template<typename T>
string FileInfo<T>::findMatch(string alias){
    for(auto &pfi : fileInfo) if(pfi.fileName == alias) return pfi.fileName;
    return "";
} 

template<typename T>
T FileInfo<T>::getSize() const {return size;}

template<typename T>
T FileInfo<T>::getHash1() const {return hash1;}

template<typename T>
T FileInfo<T>::getHash2() const {return hash2;}

template<typename T>
set<PeerFileInfo> FileInfo<T>::getFileInfo() {return fileInfo;}

template<typename T>
string FileInfo<T>::serialize() const {
    string rsp = to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size); 
    for(auto &pf : fileInfo) rsp += " " + pf.serialize();
    return rsp;
}

template<typename T>
ll FileInfo<T>::getNumberOfPeersWithFile() {
    return fileInfo.size();
}

template<typename T>
FileInfo<T> FileInfo<T>::deserialize(const string& data) {
    istringstream ss(data);
    string token;
    getline(ss, token, ',');
    T h1 = (stoll(token)); 
    getline(ss, token, ',');
    T h2 = (stoll(token)); 
    getline(ss, token, ' '); 
    T sz = (stoll(token)); 
    FileInfo<T> newFileInfo(h1, h2, sz);
    while (getline(ss, token, ' ')) {
        PeerFileInfo peerFileInfo = PeerFileInfo::deserialize(token);
        newFileInfo.knownAs(peerFileInfo);
    }
    return newFileInfo;
}

template<typename T>
bool FileInfo<T>::operator<(const FileInfo& other) const {
    if(size < other.getSize()) return 1;
    if(hash1 < other.getHash1()) return 1;
    return hash2 < other.getHash2();
}

template<typename T>
bool FileInfo<T>::operator==(const FileInfo& other) const {
    return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
}

#endif
