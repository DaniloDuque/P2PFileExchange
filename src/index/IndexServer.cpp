#pragma once
#include "util.h"
#include "common/server/TCPServer.cpp"
#include "index/fileindex/FileIndex.cpp"
#include "index/file/FileInfo.cpp"
#include "dto/NewPeerDTO.cpp"
#include "heartbeat/HeartbeatManager.cpp"

class IndexServer final : public TCPServer {
    FileIndex index;
    HeartbeatManager heartbeat;

    void addPeer(const NewPeerDTO& dto) {
        index.addPeer(dto);
        heartbeat.updatePeer(dto.ip, dto.port);
    }

    vector<pair<string, FileInfo*>> findMatches(const string& alias) const {
        return index.find(alias);
    }

    static string getFileMatchesResponse(vector<pair<string, FileInfo*>> matches) {
        if(matches.empty()) return "1";
        string rsp = "0";
        for(auto &[fileName, fileInfo] : matches){
            rsp += " " + fileName + ',' + to_string(fileInfo->getSize()); 
        }
        return rsp;
    }

    static FileInfo* searchFileInMatches(vector<pair<string, FileInfo*>> matches, const string& fileName, const ll size) {
        for (auto &[name, fileInfo] : matches) {
            if (name == fileName && fileInfo->getSize() == size) return fileInfo;
        }return nullptr; 
    }

    void handleClient(const int client_socket) override {
        logger.info("Request received!");
        const string request = toLower(readSingleBuffer(client_socket));
        if(request.empty()) return;
        if(request[0]=='1') handleAddPeer(request.substr(2, request.size()));
        if(request[0]=='2') handleFileRequest(request.substr(2, request.size()), client_socket);
        close(client_socket);
    }

    void handleAddPeer(const string& request) {
        logger.info("New Peer registration started");
        const NewPeerDTO peerFiles = NewPeerDTO::deserialize(request);
        addPeer(peerFiles);
        logger.info("New Peer registration completed - " + peerFiles.ip + ":" + to_string(peerFiles.port) + " added to index");
    }

    void handleFileRequest(const string& filename, const int client_socket) const {
        logger.info("Searching for " + filename + " in the network");
        const vector<pair<string, FileInfo*>> matches = findMatches(filename);
        string rsp = getFileMatchesResponse(matches);
        sendBytes(client_socket, rsp); 
        if (rsp == "1") return;
        const string request = toLower(readSingleBuffer(client_socket));  // rqst -> "name size"
        const vector<string> parts = split(request, ' ');
        if (parts.size() != 2) {
            logger.error("Invalid request format");
            sendBytes(client_socket, rsp = "1");
            return;
        }
        const string& actualName = parts[0];
        const ll size = stoll(parts[1]);
        const FileInfo* requestedFile = searchFileInMatches(matches, actualName, size);
        if (requestedFile == nullptr) { 
            rsp = "1";
            sendBytes(client_socket, rsp);
            return;
        }
        rsp = "0 " + requestedFile->serialize();
        sendBytes(client_socket, rsp);
        receiveAcknowledge(client_socket);
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
