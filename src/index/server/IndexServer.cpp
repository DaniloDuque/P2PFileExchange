#pragma once
#include "util.h"
#include "common/server/TCPServer.cpp"
#include "index/fileindex/FileIndex.cpp"
#include "index/file/FileInfo.cpp"
#include "dto/NewPeerDTO.cpp"
#include "dto/RemovePeerDTO.cpp"
#include "../heartbeat/HeartbeatManager.cpp"
#include "dto/SearchFileDTO.cpp""
#include "dto/RequestFileDTO.cpp"
#include "constants.h"
#include <random>

class IndexServer final : public TCPServer {
    FileIndex index;
    HeartbeatManager heartbeat;

    void addPeer(const NewPeerDTO& dto) {
        index.addPeer(dto);
        heartbeat.updatePeer(dto.ip, dto.port);
    }

    void removePeer(const RemovePeerDTO& dto) {
        heartbeat.removePeer(dto.ip, dto.port);
    }

    vector<pair<string, shared_ptr<FileInfo>>> findMatches(const string& alias) const {
        return index.find(alias);
    }

    shared_ptr<FileInfo> searchFile(const ll hash1, const ll hash2, const ll size) const {
        return index.find(hash1, hash2, size);
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
        const NewPeerDTO peerFiles = NewPeerDTO::deserialize(request);
        logger.info("New Peer indexing started - "  + peerFiles.ip + ":" + to_string(peerFiles.port));
        addPeer(peerFiles);
    }

    void handleFileRequest(const string& request, const int client_socket) const {
        const auto [hash1, hash2, size] = RequestFileDTO::deserialize(request);
        logger.info("Peer requested file - " + to_string(hash1) + " " + to_string(hash2) + " " + to_string(size));
        const auto requested_file_info = searchFile(hash1, hash2, size);
        string rsp = OK;
        if (requested_file_info != nullptr) rsp += " " + requested_file_info->serialize();
        sendBytes(client_socket, rsp);
        receiveAcknowledge(client_socket);
    }

    void handleRemovePeer(const string& request) {
        const auto remove_peer_dto = RemovePeerDTO::deserialize(request);
        logger.info("Peer removal started - " + remove_peer_dto.ip + ":" + to_string(remove_peer_dto.port));
        removePeer(remove_peer_dto);
    }

    void handleFileSearch(const string& request, const int client_socket) const {
        const auto file_search_dto = SearchFileDTO::deserialize(request);
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
        heartbeat.setDeadPeerCallback([this](const string& ip, const int port) {
            index.removePeer(ip, port);
        });
        heartbeat.start();
        TCPServer::run();
    }
    
    ~IndexServer() override {
        heartbeat.stop();
    }
};
