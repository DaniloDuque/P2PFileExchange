#include "IndexServer.h"
#include "FileMapping/FileInfo.h"

template<typename T>
void IndexServer<T>::addPeer(vector<FileInfo<T>> peerFiles){
    for(auto &pf : peerFiles) index.update(pf);
}

template<typename T>
FileInfo<T> IndexServer<T>::findFile(string alias){
    return index.find(alias);
}