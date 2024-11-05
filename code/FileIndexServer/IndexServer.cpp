#include "IndexServer.h"
#include "FileMapping/FileIndex.h"
#include "FileMapping/FileInfo.h"

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
    uchar buffer[BUFFER_SIZE] = {};
    read(client_socket, buffer, BUFFER_SIZE);
    std::string hello = "Hello from index server";
    send(client_socket, hello.c_str(), hello.size(), 0);
    std::cout << "Message Sent!" << std::endl;
    close(client_socket);
    std::cout << "Managing client on index server!" << std::endl;
}
