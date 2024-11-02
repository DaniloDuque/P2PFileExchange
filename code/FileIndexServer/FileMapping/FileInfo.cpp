#include "FileInfo.h"

template<typename T>
FileInfo<T>::FileInfo(T h1, T h2, T sz, string alias) 
    : hash1(h1), hash2(h2), size(sz) {
    knownAs(alias);  
}

template<typename T>
FileInfo<T>::FileInfo(T h1, T h2, T sz) : hash1(h1), hash2(h2), size(sz) {}

template<typename T>
void FileInfo<T>::knownAs(string alias) {
    aka.insert(alias);
}

template<typename T>
bool FileInfo<T>::operator==(const FileInfo& other) const {
    return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
}

template<typename T>
bool FileInfo<T>::operator<(const FileInfo& other) const {
    if(size < other.size()) return 1;
    if(hash1 < other.hash1) return 1;
    return hash2 < other.hash2;
}
