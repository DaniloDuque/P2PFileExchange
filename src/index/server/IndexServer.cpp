#pragma once
#include "util.h"
#include "common/server/TCPServer.cpp"
#include "index/fileindex/FileIndex.cpp"
#include "index/file/FileInfo.cpp"
#include "dto/AddPeerDTO.cpp"
#include "dto/RemovePeerDTO.cpp"
#include "../heartbeat/HeartbeatManager.cpp"
#include "dto/FileSearchDTO.cpp""
#include "dto/FileRequestDTO.cpp"
#include "constants.h"
#include <random>

class IndexServer final : public TCPServer {
    FileIndex index;
    HeartbeatManager heartbeat;

    void addPeer(const AddPeerDTO& dto) {
        index.addPeer(dto);
        heartbeat.updatePeer(dto.peer);
    }

    void removePeer(const RemovePeerDTO& dto) {
        heartbeat.removePeer(dto.peer);
    }

    vector<pair<string, shared_ptr<FileInfo>>> findMatches(const string& alias) const {
        return index.find(alias);
    }

    shared_ptr<FileInfo> searchFile(const FileDescriptor& descriptor) const {
        return index.find(descriptor);
    }

    static string getFileMatchesResponse(vector<pair<string, shared_ptr<FileInfo>>> matches) {
        if(matches.empty()) return ERR;

        random_device rd;
        mt19937 g(rd());
        ranges::shuffle(matches, g);

        string rsp = OK;
        const size_t limit = min<size_t>(RESPONSE_LIST_SIZE, matches.size());
        for(size_t i = 0; i < limit; ++i) {
            const auto& [fileName, fileInfo] = matches[i];
            rsp += " " + fileName + ',' + to_string(fileInfo->getSize());
        }
        return rsp;
    }

    void handleAddPeer(const string& request) {
        const AddPeerDTO new_peer = AddPeerDTO::deserialize(request);
        logger.info("New Peer indexing started - "  + new_peer.peer.ip + ":" + to_string(new_peer.peer.port));
        addPeer(new_peer);
    }

    void handleFileRequest(const string& request, const int client_socket) const {
        const auto [descriptor] = FileRequestDTO::deserialize(request);
        logger.info("Peer requested file - " + to_string(descriptor.hash1) + " " + to_string(descriptor.hash2) + " " + to_string(descriptor.size));
        const auto requested_file_info = searchFile(descriptor);
        string rsp = OK;
        if (requested_file_info != nullptr) rsp += " " + requested_file_info->serialize();
        sendBytes(client_socket, rsp);
        receiveAcknowledge(client_socket);
    }

    void handleRemovePeer(const string& request) {
        const auto dto = RemovePeerDTO::deserialize(request);
        logger.info("Peer removal started - " + dto.peer.ip + ":" + to_string(dto.peer.port));
        removePeer(dto);
    }

    void handleFileSearch(const string& request, const int client_socket) const {
        const auto file_search_dto = FileSearchDTO::deserialize(request);
        logger.info("Searching for " + file_search_dto.filename + " in the network");
        const vector<pair<string, shared_ptr<FileInfo>>> matches = findMatches(file_search_dto.filename);
        const string rsp = getFileMatchesResponse(matches);
        sendBytes(client_socket, rsp);
    }

    void handleClient(const int client_socket) override {
        logger.info("Request received!");
        const string request = toLower(readSingleBuffer(client_socket));
        if(request.empty()) return;
        const auto data = request.substr(2, request.size());
        if (request[0] == ADD_PEER) handleAddPeer(data);
        if (request[0] == FILE_REQUEST) handleFileRequest(data, client_socket);
        if (request[0] == REMOVE_PEER) handleRemovePeer(data);
        if (request[0] == FILE_SEARCH) handleFileSearch(data, client_socket);
        close(client_socket);
    }

public:
    explicit IndexServer(const int port): TCPServer(port) {}
    
    void run() override {
        heartbeat.setDeadPeerCallback([this](const PeerDescriptor& peer) {
            index.removePeer(peer);
        });
        heartbeat.start();
        TCPServer::run();
    }
    
    ~IndexServer() override {
        heartbeat.stop();
    }
};
