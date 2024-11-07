#ifndef ADD_PEER_HEADER
#define ADD_PEER_HEADER

#include "PeerFileDTO.h"
#include "FileExchange/PeerServer.h"

void addPeer(string &indexIp, string &indexPort, string &directory){

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Error creating socket" << endl;
        return;
    }
    
    struct hostent *server = gethostbyname(indexIp.c_str());
    if (server == NULL) {
        cerr << "Error: could not resolve server address" << endl;
        close(clientSocket);
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(stoi(indexPort));  
    memcpy(&serverAddress.sin_addr.s_addr, server->h_addr_list[0], server->h_length);  

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error connecting to the server" << endl;
        close(clientSocket);
        return;
    }
    
    vector<PeerFileDTO<ll>> list = fileDirectoryReader(directory);
    string package = "1 ";

    for (int i = 0; i < (int)list.size(); i++) {
        package += list[i].serialize();
        if (i != (int)list.size() - 1) {
            package += ' ';
        }
    }
    
    if (send(clientSocket, package.c_str(), package.size(), 0) < 0) {
        cerr << "Error sending the package" << endl;
        close(clientSocket);
        return;
    }

    puts("Package sent!");
    close(clientSocket);

}

#endif
