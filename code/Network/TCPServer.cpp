#include "TCPServer.h"

TCPServer::TCPServer(int p) : port(p) {} 

void TCPServer::run() {

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int bufferSize = BUFFER_SIZE;

    if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) < 0) {
        cerr << "Error setting receive buffer size: " << strerror(errno) << endl;
    }
    if (setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize)) < 0) {
        cerr << "Error setting send buffer size: " << strerror(errno) << endl;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    listen(serverSocket, 15);

    char ip_str[INET_ADDRSTRLEN];  
    inet_ntop(AF_INET, &serverAddress.sin_addr, ip_str, INET_ADDRSTRLEN);  
    cout << "Server listening on port " << port << endl;

    while (true) {
        int new_socket = accept(serverSocket, nullptr, nullptr);
        if (new_socket < 0) {
            cerr << "Error in accept()" << endl;
            continue;
        }

        thread(&TCPServer::handleClient, this, new_socket).detach();
    }
}
