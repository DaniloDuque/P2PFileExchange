#include "FileIndex.h"
#include "FileInfo.h"

template<typename T>
FileIndex<T>::FileIndex() {
}

template<typename T>
void FileIndex<T>::update(PeerFileDTO<T> dto){
    FileInfo<T> cur(dto.h1, dto.h2, dto.size);
    auto it = info.find(cur);
    if(it!=info.end()){
        it->second->knownAs(dto.alias, dto.info);
        return;
    }
    info[cur]=make_shared<FileInfo<T>>(dto.h1,dto.h2,dto.size,dto.alias,dto.info);
} 

template<typename T>
FileInfo<T>* FileIndex<T>::find(string alias) {
    for (auto &p : info) {
        if (p.second->exists(alias)) return p.second.get(); 
    }
    return nullptr; 
}
