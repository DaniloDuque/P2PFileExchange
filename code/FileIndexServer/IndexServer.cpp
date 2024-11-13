#include "IndexServer.h"
#include "FileMapping/FileIndex.h"
#include "FileMapping/FileInfo.h"
#include "../util.h"
#include "PeerFileDTO.h"
#include <unistd.h>

template<typename T>
IndexServer<T>::IndexServer(int p) : TCPServer(p) {}

template<typename T>
void IndexServer<T>::addPeer(vector<PeerFileDTO<T>> peerFiles){
    for(auto &pf : peerFiles) puts("File indexed!"), index.update(pf);
}

template<typename T>
vector<FileInfo<T>*> IndexServer<T>::findFiles(string alias){
    return index.find(alias);
}
//
// template<typename T>
// string IndexServer<T>::sendMatches(vector<FileInfo<T>*> &matches, string &filename, int client_socket){
//     cout<<"Found "<<matches.size()<<" matches for "<<filename<<" in the network"<<endl;
//     if(matches.empty()) return "1";
//     string rsp = "0 ";
//     for(auto &f : matches){
//         rsp += f.
//     }
//     
// }
//
template<typename T>
void IndexServer<T>::handleFindFile(string fileName, int client_socket){
    vector<FileInfo<T>*> files = findFiles(fileName);
    string rsp;
    if(files.empty()) rsp = "1";
    rsp = "0 ";
    for(auto &f : files) rsp += f->serialize(); 
    sendBytes(client_socket, rsp);
    string rslt = receiveAcknowledge(client_socket);
}

template<typename T>
void IndexServer<T>::handleAddPeer(string rqst){
    vector<PeerFileDTO<T>> files;
    istringstream stream(rqst);
    string file;
    while(stream >> file){
        files.push_back(PeerFileDTO<T>::deserialize(file));
    }
    addPeer(files);
}

template<typename T>
void IndexServer<T>::handleClient(int client_socket) {
    puts("Request received!");
    string rqst = toLower(readSingleBuffer(client_socket));
    if(rqst.empty()) return;
    if(rqst[0]=='1') handleAddPeer(rqst.substr(2, rqst.size()));
    if(rqst[0]=='2') handleFindFile(rqst.substr(2, rqst.size()), client_socket);
    close(client_socket);
}
