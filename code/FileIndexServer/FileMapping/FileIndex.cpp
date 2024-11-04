#include "FileIndex.h"
#include "FileInfo.h"

template<typename T>
void FileIndex<T>::update(PeerFileDTO dto){
    FileInfo<T> cur(dto.h1, dto.h2, dto.size);
    auto it = info.find(cur);
    if(it!=info.end()){
        it->second.knownAs(dto.alias, dto.pinfo);
        return;
    }
    info[cur]=make_shared<FileInfo<T>>(dto.h1,dto.h2,dto.sz,dto.alias,dto.pinfo);
} 

template<typename T>
FileInfo<T> FileIndex<T>::find(string alias){
    for(auto &p : info){
        if(p.snd.exists(alias)) p.snd;
    }return r;
}
