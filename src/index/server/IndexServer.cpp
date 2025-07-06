#pragma once
#include "util.h"
#include "common/server/TCPServer.cpp"
#include "index/fileindex/FileIndex.cpp"
#include "index/file/FileInfo.cpp"
#include "dto/NewPeerDTO.cpp"
#include "dto/RemovePeerDTO.cpp"
#include "../heartbeat/HeartbeatManager.cpp"

class IndexServer final : public TCPServer {
    FileIndex index;
    HeartbeatManager heartbeat;

    void addPeer(const NewPeerDTO& dto) {
        index.addPeer(dto);
        heartbeat.updatePeer(dto.ip, dto.port);
    }

    vector<pair<string, shared_ptr<FileInfo>>> findMatches(const string& alias) const {
        return index.find(alias);
    }

    static string getFileMatchesResponse(vector<pair<string, shared_ptr<FileInfo>>> matches) {
        if(matches.empty()) return "1";
        string rsp = "0";
        for(auto &[fileName, fileInfo] : matches){
            rsp += " " + fileName + ',' + to_string(fileInfo->getSize()); 
        }
        return rsp;
    }

    static shared_ptr<FileInfo> searchFileInMatches(vector<pair<string, shared_ptr<FileInfo>>> matches, const string& fileName, const ll size) {
        for (auto &[name, fileInfo] : matches) {
            if (name == fileName && fileInfo->getSize() == size) return fileInfo;
        }return nullptr; 
    }

    void handleClient(const int client_socket) override {
        logger.info("Request received!");
        const string request = toLower(readSingleBuffer(client_socket));
        if(request.empty()) return;
        const auto data = request.substr(2, request.size());
        if (request[0]=='1') handleAddPeer(data);
        if (request[0]=='2') handleFileRequest(data, client_socket);
        if (request[0]=='3') handleRemovePeer(data);
        close(client_socket);
    }

    void handleRemovePeer(const string& request) {
        const auto [ip, port] = RemovePeerDTO::deserialize(request);
        logger.info("Peer removal started - " + ip + ":" + to_string(port));
        heartbeat.removePeer(ip, port);
    }

    void handleAddPeer(const string& request) {
        const NewPeerDTO peerFiles = NewPeerDTO::deserialize(request);
        logger.info("New Peer indexing started - "  + peerFiles.ip + ":" + to_string(peerFiles.port));
        addPeer(peerFiles);
    }

    void handleFileRequest(const string& filename, const int client_socket) const {
        logger.info("Searching for " + filename + " in the network");
        const vector<pair<string, shared_ptr<FileInfo>>> matches = findMatches(filename);
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
        const shared_ptr<FileInfo> requestedFile = searchFileInMatches(matches, actualName, size);
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
