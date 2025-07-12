#pragma once
#include <Server.h>
#include <common/bytestream/TCPStream.cpp>
#include <logger/Logger.h>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <atomic>
#include <constants.h>

class TCPServer : public Server {
    std::atomic<bool> is_running = false;
    int serverSocket = -1;

public:
    TCPServer(const shared_ptr<ByteStream> &stream, const int port)
        : Server(stream, port) {
    }

    void run() override {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            logger.error("Failed to create socket: " + std::string(strerror(errno)));
            return;
        }

        constexpr int opt = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
            logger.warn("Error setting SO_REUSEADDR: " + std::string(strerror(errno)));

        constexpr int bufferSize = BUFFER_SIZE;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) < 0)
            logger.warn("Error setting receive buffer size: " + std::string(strerror(errno)));

        if (setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize)) < 0)
            logger.warn("Error setting send buffer size: " + std::string(strerror(errno)));

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        if (::bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
            logger.error("Bind failed: " + std::string(strerror(errno)));
            close(serverSocket);
            return;
        }

        if (listen(serverSocket, 15) < 0) {
            logger.error("Listen failed: " + std::string(strerror(errno)));
            close(serverSocket);
            return;
        }

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &serverAddress.sin_addr, ip_str, INET_ADDRSTRLEN);
        logger.info("Server listening on port: " + std::to_string(port));

        is_running = true;
        while (is_running) {
            int new_socket = accept(serverSocket, nullptr, nullptr);
            if (!is_running) break;

            if (new_socket < 0) {
                logger.error("Error in accept(): " + std::string(strerror(errno)));
                continue;
            }

            std::thread(&TCPServer::handle_client, this, new_socket).detach();
        }

        close(serverSocket);
        logger.info("Server stopped");
    }

    void stop() override {
        logger.info("Stopping server...");
        is_running = false;
        if (serverSocket >= 0) {
            shutdown(serverSocket, SHUT_RDWR);
            close(serverSocket);
        }
    }
};
