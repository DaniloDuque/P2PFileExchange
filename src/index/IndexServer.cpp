#pragma once
#include "../util.h"
#include "../common/TCPServer.cpp"
#include "fileindex/FileIndex.cpp"
#include "file/FileInfo.cpp"
#include "dto/NewPeerDTO.cpp"

class IndexServer : public TCPServer {
private:
    FileIndex index;

    void addPeer(NewPeerDTO dto) {
        index.addPeer(dto);
    }

    vector<pair<string, FileInfo*>> findMatches(string alias) {
        return index.find(alias);
    }

    string getFileMatchesResponse(vector<pair<string, FileInfo*>> matches) {
        if(matches.empty()) return "1";
        string rsp = "0";
        for(auto &[fileName, fileInfo] : matches){
            rsp += " " + fileName + ',' + to_string(fileInfo->getSize()); 
        }
        return rsp;
    }

    FileInfo* searchFileInMatches(vector<pair<string, FileInfo*>> matches, string& fileName, ll size) {
        for (auto &[name, fileInfo] : matches) {
            if (name == fileName && fileInfo->getSize() == size) return fileInfo;
        }return nullptr; 
    }

    void handleClient(int client_socket) override {
        logger.info("Request received!");
        string rqst = toLower(readSingleBuffer(client_socket));
        if(rqst.empty()) return;
        if(rqst[0]=='1') handleAddPeer(rqst.substr(2, rqst.size()));
        if(rqst[0]=='2') handleFileRequest(rqst.substr(2, rqst.size()), client_socket);
        close(client_socket);
    }

    void handleAddPeer(string request) {
        logger.info("New Peer registration started");
        NewPeerDTO peerFiles = NewPeerDTO::deserialize(request);
        addPeer(peerFiles);
        logger.info("New Peer registration completed - " + peerFiles.ip + ":" + to_string(peerFiles.port) + " added to index");
    }

    void handleFileRequest(string filename, int client_socket) {
        logger.info("Searching for " + filename + " in the network");
        vector<pair<string, FileInfo*>> matches = findMatches(filename);
        string rsp = getFileMatchesResponse(matches);
        sendBytes(client_socket, rsp); 
        if (rsp == "1") return;
        string rqst = toLower(readSingleBuffer(client_socket));  // rqst -> "name size"
        vector<string> parts = split(rqst, ' ');  
        if (parts.size() != 2) {
            logger.error("Invalid request format");
            sendBytes(client_socket, rsp = "1");
            return;
        }
        string actualName = parts[0];  
        ll size = stoll(parts[1]);    
        FileInfo* requestedFile = searchFileInMatches(matches, actualName, size);
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
    IndexServer(int port): TCPServer(port) {}
};
