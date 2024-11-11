#include "TCPServer.h"


TCPServer::TCPServer(int p, string addr) : port(p), ip_addr(addr) {} 

void TCPServer::run() {

    struct sockaddr_in address;
    int server_fd, opt = 1, addrlen = sizeof(address);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        cerr << "Error creating socket" << endl;
        exit(EXIT_FAILURE);
    }
    
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    
    if (inet_pton(AF_INET, ip_addr.c_str(), &address.sin_addr) <= 0) {
        cerr << "Invalid address/ Address not supported" << endl;
        exit(EXIT_FAILURE);
    }

    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        cerr << "Error in bind()" << endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0) {
        cerr << "Error in listen()" << endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char ip_str[INET_ADDRSTRLEN];  
    inet_ntop(AF_INET, &address.sin_addr, ip_str, INET_ADDRSTRLEN);  
    cout << "Server on IP " << ip_str << " listening on port " << port << endl;

    while (true) {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            cerr << "Error in accept()" << endl;
            continue;
        }

        thread(&TCPServer::handleClient, this, new_socket).detach();
    }
}
