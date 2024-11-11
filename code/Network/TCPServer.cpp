#include "TCPServer.h"

TCPServer::TCPServer(int p) : port(p) {} 

void TCPServer::run() {

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    listen(serverSocket, 5);

    char ip_str[INET_ADDRSTRLEN];  
    inet_ntop(AF_INET, &serverAddress.sin_addr, ip_str, INET_ADDRSTRLEN);  
    cout << "Server on IP " << ip_str << " listening on port " << port << endl;

    while (true) {
        int new_socket = accept(serverSocket, nullptr, nullptr);
        if (new_socket < 0) {
            cerr << "Error in accept()" << endl;
            continue;
        }

        thread(&TCPServer::handleClient, this, new_socket).detach();
    }
}
