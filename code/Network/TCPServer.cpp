#include "TCPServer.h"

TCPServer::TCPServer(int p) : port(p) {} 

void TCPServer::handleClient(int client_socket) {
    byte buffer[BUFFER_SIZE] = {};
    read(client_socket, buffer, BUFFER_SIZE);
    
    std::string hello = "Hello from server";
    send(client_socket, hello.c_str(), hello.size(), 0);
    
    std::cout << "Message Sent!" << std::endl;
    
    close(client_socket);
}

void TCPServer::run() {
    struct sockaddr_in address;
    int server_fd, opt = 1, addrlen = sizeof(address);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "Error creating socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Error in bind()" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Error in listen()" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            std::cerr << "Error in accept()" << std::endl;
            continue;
        }

        std::thread(&TCPServer::handleClient, this, new_socket).detach();
    }
}

