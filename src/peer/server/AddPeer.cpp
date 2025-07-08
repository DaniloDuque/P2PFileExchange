#pragma once
#include "dto/NewPeerDTO.cpp"
#include "PeerServer.cpp"
#include "logger/Logger.h"

void addPeer(const string &port, const string &ip, const string &indexIp, const string &indexPort, const string &directory) {
    int clientSocket = 0;
    for(int i = 0; i < 5; i++) {
        clientSocket = PeerServer::connectToServer(indexIp, indexPort);
        if(clientSocket >= 0) break;
        this_thread::sleep_for(chrono::milliseconds(200));
    }
    if (clientSocket < 0) {
        logger.error("Failed to connect to index server after retries");
        return;
    }
    NewPeerDTO dto;
    dto.ip = ip;
    dto.port = stoi(port);
    for(const auto& f : fileDirectoryReader(directory)) dto.peerFiles.push_back(f);
    const string package = to_string(ADD_PEER) + " " + dto.serialize();
    if (send(clientSocket, package.c_str(), package.size(), 0) < 0) {
        logger.error("Error sending the package");
        close(clientSocket);
        return;
    }
    logger.info("Package sent!");
    close(clientSocket);
}
