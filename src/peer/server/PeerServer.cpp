#pragma once
#include "../../util.h"
#include "../../common/TCPServer.cpp"
#include "../dto/FileRequestDTO.cpp"
#include "../hash/FileReader.cpp"
#include "../../logger/Logger.h"

class PeerServer : public TCPServer {
private:
    string path;
    set<FileDTO> sharedFiles;

    void handleClient(int peerSocket) override {
        string rqst = readSingleBuffer(peerSocket);
        logger.info("Client on socket: " + to_string(peerSocket));
        if(rqst.size()) sendFilePart(peerSocket, FileRequestDTO::deserialize(rqst));
    }

    void sendFilePart(int peerSocket, FileRequestDTO request) {
        string filePath = path + "/" + searchFile(request);
        FILE* file = fopen(filePath.c_str(), "rb");
        if (!file) {
            logger.error("Error opening file: " + filePath);
            return;
        }
        fseek(file, request.startByte, SEEK_SET);
        char buffer[BUFFER_SIZE];
        ll leftBytes = request.chunkSize;
        while (leftBytes > 0) {
            size_t bytesToRead = min(leftBytes, static_cast<ll>(BUFFER_SIZE));
            size_t bytesRead = fread(buffer, 1, bytesToRead, file);
            if (bytesRead > 0) {
                string data(buffer, bytesRead);
                if(!sendBytes(peerSocket, data)) break;
                leftBytes -= bytesRead;
            } else {
                logger.error("Error reading from file or end of file.");
                break;
            }
        }
        fclose(file);
    }

    string searchFile(FileRequestDTO dto) {
        auto it = sharedFiles.find(FileDTO{dto.hash1, dto.hash2, dto.size, ""});
        if(it == sharedFiles.end()) return "";  
        return it->fileName;  
    }

public:

    static int connectToServer(string serverIp, string serverPort) {
        logger.info("Establishing connection to " + serverIp + ":" + serverPort);
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            logger.error("Error creating socket");
            return -1;
        }
        ll bufferSize = BUFFER_SIZE;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) < 0) 
            logger.warn("Error setting receive buffer size: " + string(strerror(errno)));
        
        if (setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize)) < 0)
            logger.warn("Error setting send buffer size: " + string(strerror(errno)));
        
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(stoi(serverPort));
        if (inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr) <= 0) {
            logger.error("Invalid address or address not supported");
            close(serverSocket);
            return -1;
        }
        if (connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
            logger.error("Error connecting to the server: " + string(strerror(errno)));
            close(serverSocket);
            return -1;
        }
        return serverSocket;
    }

    PeerServer(int port, string &directory): TCPServer(port), path(directory) {
        vector<FileDTO> list = fileDirectoryReader(directory);
        for(auto &pf : list) sharedFiles.emplace(pf.hash1, pf.hash2, pf.size, pf.fileName);
    }

};

