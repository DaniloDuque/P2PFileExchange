#pragma once
#include "../dto/NewPeerDTO.cpp"
#include "PeerServer.cpp"

void addPeer(string &port, string &ip, string &indexIp, string &indexPort, string &directory){
    int clientSocket = PeerServer::connectToServer(indexIp, indexPort);
    NewPeerDTO dto;
    dto.ip = ip;
    dto.port = stoi(port);
    for(auto f : fileDirectoryReader(directory)) dto.peerFiles.push_back(f);
    string package = "1 " + dto.serialize();
    if (send(clientSocket, package.c_str(), package.size(), 0) < 0) {
        cerr << "Error sending the package" << endl;
        close(clientSocket);
        return;
    }

    puts("Package sent!");
    close(clientSocket);

}
