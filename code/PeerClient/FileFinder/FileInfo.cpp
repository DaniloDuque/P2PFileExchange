#include "FileInfo.h"
#include "PeerInfo.h"
#include <sstream>
#include <string>

template<typename T>
FileInfo<T>::FileInfo(T h1, T h2, T sz) : hash1(h1), hash2(h2), size(sz) {}

template<typename T>
FileInfo<T> FileInfo<T>::deserialize(string data) {
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
        fileInfo.addr.push_back(PeerInfo::deserialize(token));
    }
    return fileInfo;
}
