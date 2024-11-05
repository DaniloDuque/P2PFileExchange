#include "IndexServer.h"
#include "FileMapping/FileIndex.h"
#include "FileMapping/FileInfo.h"
#include "../util.h"
#include "PeerFileDTO.h"

template<typename T>
IndexServer<T>::IndexServer(int p) : TCPServer(p) {}

template<typename T>
void IndexServer<T>::addPeer(vector<PeerFileDTO<T>> peerFiles){
    for(auto &pf : peerFiles) puts("File indexed!"), index.update(pf);
}

template<typename T>
FileInfo<T> IndexServer<T>::findFile(string alias){
    return index.find(alias);
}

template<typename T>
void IndexServer<T>::handleFindFile(string rqst, int client_socket){
    cout<<client_socket<<' '<<rqst<<endl;
}

template<typename T>
void IndexServer<T>::handleAddPeer(string rqst){
    vector<PeerFileDTO<T>> files;
    istringstream stream(rqst);
    string file;
    while(stream >> file) files.push_back(PeerFileDTO<T>::deserialize(file));
    addPeer(files);
}

template<typename T>
void IndexServer<T>::handleClient(int client_socket) {
    puts("Request received!");
    char buffer[BUFFER_SIZE] = {};
    read(client_socket, buffer, BUFFER_SIZE);
    string rqst = buffer;
    if(rqst.empty()) return;
    if(rqst[0]=='1') handleAddPeer(rqst.substr(1, rqst.size()));
    if(rqst[0]=='2') handleFindFile(rqst.substr(1, rqst.size()), client_socket);
}

