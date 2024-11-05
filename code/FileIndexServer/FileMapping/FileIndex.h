#ifndef FILE_INDEX_HEADER
#define FILE_INDEX_HEADER

#include "FileInfo.h"
#include <iostream>
#include <map>
#include <memory>
#include "../PeerFileDTO.h"

template<typename T>
class FileIndex {

private:
    map<FileInfo<T>, shared_ptr<FileInfo<T>>> info;
public:
    FileIndex();
    FileInfo<T>* find(string); 
    void update(PeerFileDTO<T>);
};

#include "FileIndex.cpp"

#endif 
