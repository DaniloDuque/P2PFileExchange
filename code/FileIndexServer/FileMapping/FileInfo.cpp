#include "FileInfo.h"
#include <sstream>
#include <string>

template<typename T>
FileInfo<T>::FileInfo(T h1, T h2, T sz, string alias, PeerInfo info) 
    : hash1(h1), hash2(h2), size(sz) {
    knownAs(alias, info);  
}

template<typename T>
void FileInfo<T>::knownAs(string alias, PeerInfo info) {
    aka.insert(alias);
    addr.push_back(info);
}

template<typename T>
FileInfo<T>::FileInfo(T h1, T h2, T sz) : hash1(h1), hash2(h2), size(sz) {}

template<typename T>
T FileInfo<T>::getSize() const { 
    return size;
}

template<typename T>
T FileInfo<T>::getHash1() const { 
    return hash1;
}

template<typename T>
T FileInfo<T>::getHash2() const { 
    return hash2;
}

template<typename T>
bool FileInfo<T>::operator<(const FileInfo& other) const {
    if(size < other.getSize()) return 1;
    if(hash1 < other.getHash1()) return 1;
    return hash2 < other.getHash2();
}

template<typename T>
bool FileInfo<T>::exists(string alias){
    return (aka.find(alias)!=aka.end());
}

template<typename T>
bool FileInfo<T>::operator==(const FileInfo& other) const {
    return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
}

template<typename T>
string FileInfo<T>::serialize() const {
    string rsp = to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size); 
    for(auto &pf : addr) rsp += " " + pf.serialize();
    return rsp;
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
    FileInfo<T> fileInfo(h1, h2, sz);
    while (getline(ss, token, ' ')) {
        PeerInfo peerInfo = PeerInfo::deserialize(token);
        fileInfo.addr.push_back(peerInfo);
    }
    return fileInfo;
}

