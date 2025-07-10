#pragma once
#include "util.h"
#include "common/server/TCPServer.cpp"
#include "dto/DownloadFileChunkDTO.cpp"
#include "../client/FileReader.cpp"
#include "logger/Logger.h"

class PeerServer final : public TCPServer {
    string path;
    set<IndexedFileDescriptor> sharedFiles;

    void handleClient(const int peerSocket) override {
        const string request = readSingleBuffer(peerSocket);
        logger.info("Client on socket: " + to_string(peerSocket));
        if(!request.empty()) sendFilePart(peerSocket, DownloadFileChunkDTO::deserialize(request));
    }

    void sendFilePart(const int peerSocket, const DownloadFileChunkDTO &request) {
        const string filePath = path + "/" + searchFile(request);
        FILE* file = fopen(filePath.c_str(), "rb");
        if (!file) {
            logger.error("Error opening file: " + filePath);
            return;
        }
        fseek(file, request.startByte, SEEK_SET);
        char buffer[BUFFER_SIZE];
        unsigned ll leftBytes = request.chunkSize;
        while (leftBytes > 0) {
            const size_t bytesToRead = min(leftBytes, static_cast<unsigned ll>(BUFFER_SIZE));
            if (const size_t bytesRead = fread(buffer, 1, bytesToRead, file); bytesRead > 0) {
                if(string data(buffer, bytesRead); !send_bytes(peerSocket, data)) break;
                leftBytes -= bytesRead;
            } else {
                logger.error("Error reading from file or end of file.");
                break;
            }
        }
        fclose(file);
    }

    string searchFile(const DownloadFileChunkDTO &dto) {
        const auto it = sharedFiles.find({dto.file});
        if(it == sharedFiles.end()) return "";  
        return it->filename;
    }

public:
    static int connectToServer(const string& serverIp, const string& serverPort) {
        logger.info("Establishing connection to " + serverIp + ":" + serverPort);
        const int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            logger.error("Error creating socket");
            return -1;
        }
        constexpr
        size_t bufferSize = BUFFER_SIZE;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) < 0) 
            logger.warn("Error setting receive buffer size: " + string(strerror(errno)));
        
        if (setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize)) < 0)
            logger.warn("Error setting send buffer size: " + string(strerror(errno)));
        
        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(stoi(serverPort));
        if (inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr) <= 0) {
            logger.error("Invalid address or address not supported");
            close(serverSocket);
            return -1;
        }
        if (connect(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
            logger.error("Error connecting to the server: " + string(strerror(errno)));
            close(serverSocket);
            return -1;
        }
        return serverSocket;
    }

    PeerServer(const int port, const string &directory): TCPServer(port), path(directory) {
        for(const vector<IndexedFileDescriptor> list = fileDirectoryReader(directory); auto &pf : list)
            sharedFiles.emplace(pf.file, pf.filename);
    }

};

