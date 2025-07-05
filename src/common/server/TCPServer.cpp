#pragma once
#include "Server.h"
#include "../../logger/Logger.h"
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

class TCPServer : public Server {
public:
    TCPServer(int port): Server(port) {}

    void run() override {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            logger.error("Failed to create socket: " + std::string(strerror(errno)));
            return;
        }

        int opt = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            logger.error("Error setting SO_REUSEADDR: " + std::string(strerror(errno)));

        int bufferSize = BUFFER_SIZE;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) < 0)
            logger.error("Error setting receive buffer size: " + std::string(strerror(errno)));

        if (setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize)) < 0)
            logger.error("Error setting send buffer size: " + std::string(strerror(errno)));

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        if (::bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
            logger.error("Bind failed: " + std::string(strerror(errno)));
            close(serverSocket);
            return;
        }

        if (::listen(serverSocket, 15) < 0) {
            logger.error("Listen failed: " + std::string(strerror(errno)));
            close(serverSocket);
            return;
        }

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &serverAddress.sin_addr, ip_str, INET_ADDRSTRLEN);
        logger.info("Server listening on port: " + std::to_string(port));

        while (true) {
            int new_socket = accept(serverSocket, nullptr, nullptr);
            if (new_socket < 0) {
                logger.error("Error in accept(): " + std::string(strerror(errno)));
                continue;
            }

            std::thread(&TCPServer::handleClient, this, new_socket).detach();
        }
    }
    
};
