#include "IndexServer.h"
#include "FileMapping/FileIndex.h"
#include "FileMapping/FileInfo.h"
#include "../util.h"
#include "FileMapping/NewPeerDTO.h"
#include <filesystem>

template<typename T>
IndexServer<T>::IndexServer(int p) : TCPServer(p) {}

template<typename T>
void IndexServer<T>::addPeer(NewPeerDTO<T> dto){
    index.addPeer(dto);
}

template<typename T>
vector<pair<string, FileInfo<T>*>> IndexServer<T>::findMatches(string alias){
    return index.find(alias);
}

template<typename T>
string IndexServer<T>::getFileMatchesResponse(vector<pair<string, FileInfo<T>*>> matches){
    if(matches.empty()) return "1";
    string rsp = "0";
    for(auto &[fileName, fileInfo] : matches){
        rsp += " " + fileName + ',' + to_string(fileInfo->getSize()); 
    }
    return rsp;
}

template<typename T>
FileInfo<T>* IndexServer<T>::searchFileInMatches(vector<pair<string, FileInfo<T>*>> matches, string& fileName, T size) {
    for (auto &[name, fileInfo] : matches) {
        if (name == fileName && fileInfo->getSize() == size) return fileInfo;
    }return nullptr; 
}

template<typename T>
void IndexServer<T>::handleFileRequest(string fileName, int client_socket) {
    cout << "Searching for " << fileName << " in the network" << endl;
    vector<pair<string, FileInfo<T>*>> matches = findMatches(fileName);
    string rsp = getFileMatchesResponse(matches);
    sendBytes(client_socket, rsp); 
    if (rsp == "1") return;
    string rqst = toLower(readSingleBuffer(client_socket));  // rqst -> "name size"
    vector<string> parts = split(rqst, ' ');  
    if (parts.size() != 2) {
        cout << "Invalid request format" << endl;
        sendBytes(client_socket, rsp = "1");
        return;
    }
    string actualName = parts[0];  
    T size = stoll(parts[1]);    
    FileInfo<T>* requestedFile = searchFileInMatches(matches, actualName, size);
    if (requestedFile == nullptr) { 
        rsp = "1";
        sendBytes(client_socket, rsp);
        return;
    }
    rsp = "0 " + requestedFile->serialize();
    sendBytes(client_socket, rsp);
    receiveAcknowledge(client_socket);
}

template<typename T>
void IndexServer<T>::handleAddPeer(string rqst){
    NewPeerDTO<T> peerFiles = NewPeerDTO<T>::deserialize(rqst);
    addPeer(peerFiles);
    cout << "The files from peer at IP: " << peerFiles.ip << " (Port: " << peerFiles.port << ") have been successfully indexed." << endl;
}

template<typename T>
void IndexServer<T>::handleClient(int client_socket) {
    puts("Request received!");
    string rqst = toLower(readSingleBuffer(client_socket));
    if(rqst.empty()) return;
    if(rqst[0]=='1') handleAddPeer(rqst.substr(2, rqst.size()));
    if(rqst[0]=='2') handleFileRequest(rqst.substr(2, rqst.size()), client_socket);
    close(client_socket);
}
