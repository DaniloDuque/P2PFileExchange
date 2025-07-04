#pragma once
#include "Server.h"
#include "../logger/Logger.h"

class TCPServer : public Server {
public:
    TCPServer(int port): Server(port) {}

    void run() override {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        int bufferSize = BUFFER_SIZE;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) < 0)
            logger.error("Error setting receive buffer size: " + string(strerror(errno)));

        if (setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize)) < 0)
            logger.error("Error setting send buffer size: " + string(strerror(errno)));

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
        listen(serverSocket, 15);
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &serverAddress.sin_addr, ip_str, INET_ADDRSTRLEN);
        logger.info("Server listening on port: " + to_string(port));

        while (true) {
            int new_socket = accept(serverSocket, nullptr, nullptr);
            if (new_socket < 0) {
                logger.error("Error in accept()");
                continue;
            }

            thread(&TCPServer::handleClient, this, new_socket).detach();
        }
    }
};
