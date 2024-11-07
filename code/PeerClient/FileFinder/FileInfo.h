#ifndef FILEINFO_H
#define FILEINFO_H


#include "../../util.h"
#include "PeerInfo.h"
using namespace std;

template<typename T>
class FileInfo {
public:
    const T hash1, hash2, size;
    vector<PeerInfo> addr;
    FileInfo(T, T, T);
    static FileInfo<T> deserialize(string);
};

#include "FileInfo.cpp"

#endif 

