#pragma once
#include "util.h"
#include "dto/RemovePeerDTO.cpp"

void removePeer(const string& ip, const string& indexIp, const string& indexPort, const string& port) {
    const int indexSocket = PeerServer::connectToServer(indexIp, indexPort);
    if (indexSocket < 0) return;

    const RemovePeerDTO dto{ip, stoi(port)};
    const string request = to_string(REMOVE_PEER) + " " + dto.serialize();
    sendBytes(indexSocket, request);
    close(indexSocket);
    logger.info("Peer removal request sent to index server");
}