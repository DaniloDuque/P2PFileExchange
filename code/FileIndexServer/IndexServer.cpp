#include "IndexServer.h"
#include "FileMapping/FileIndex.h"
#include "FileMapping/FileInfo.h"
#include "../util.h"

template<typename T>
IndexServer<T>::IndexServer(int p) : TCPServer(p) {}

template<typename T>
void IndexServer<T>::addPeer(vector<PeerFileDTO<T>> peerFiles){
    for(auto &pf : peerFiles) index.update(pf);
}

template<typename T>
FileInfo<T> IndexServer<T>::findFile(string alias){
    return index.find(alias);
}

template<typename T>
void IndexServer<T>::handleClient(int client_socket) {
    char buffer[BUFFER_SIZE] = {};
    read(client_socket, buffer, BUFFER_SIZE);
    string rslt = buffer;
    for(char c : rslt) cout<<c;
    cout<<endl;
    close(client_socket);
}
