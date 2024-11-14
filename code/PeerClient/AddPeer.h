#ifndef ADD_PEER_HEADER
#define ADD_PEER_HEADER

#include "../util.h"
#include "NewPeerDTO.h"
#include "FileExchange/PeerServer.h"

template<typename T>
void addPeer(string &port, string &ip, string &indexIp, string &indexPort, string &directory){
    int clientSocket = PeerServer<T>::connectToServer(indexIp, indexPort);
    NewPeerDTO<T> dto;
    dto.ip = ip;
    dto.port = stoi(port);
    for(auto f : fileDirectoryReader<T>(directory)) dto.peerFiles.push_back(f);
    string package = "1 " + dto.serialize();
    if (send(clientSocket, package.c_str(), package.size(), 0) < 0) {
        cerr << "Error sending the package" << endl;
        close(clientSocket);
        return;
    }

    puts("Package sent!");
    close(clientSocket);

}

#endif
