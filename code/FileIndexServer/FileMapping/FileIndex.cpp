#include "FileIndex.h"
#include "FileInfo.h"

template<typename T>
void FileIndex<T>::update(T h1, T h2, T sz, string alias){
    FileInfo<T> cur(h1, h2, sz);
    auto it = info.find(cur);
    if(it!=info.end()){
        it->second.knownAs(alias);
        return;
    }
    info[cur]=make_shared<FileInfo<T>>(h1,h2,sz,alias);
} 
