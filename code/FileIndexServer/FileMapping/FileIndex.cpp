#include "FileIndex.h"
#include "FileInfo.h"

template<typename T>
void FileIndex<T>::update(T h1, T h2, T sz, string alias, PeerInfo pinfo){
    FileInfo<T> cur(h1, h2, sz);
    auto it = info.find(cur);
    if(it!=info.end()){
        it->second.knownAs(alias, pinfo);
        return;
    }
    info[cur]=make_shared<FileInfo<T>>(h1,h2,sz,alias,pinfo);
} 

template<typename T>
vector<FileInfo<T>> FileIndex<T>::find(string alias){
    for(auto &p : info){
        if(p.snd.exists(alias)) return p.snd;
    }return {};
}
