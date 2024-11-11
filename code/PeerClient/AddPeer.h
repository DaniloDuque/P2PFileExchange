#ifndef ADD_PEER_HEADER
#define ADD_PEER_HEADER

#include "PeerFileDTO.h"
#include "FileExchange/PeerServer.h"

void addPeer(string &port, string &ip, string &indexIp, string &indexPort, string &directory){

    int clientSocket = PeerServer<ll>::connectToServer(indexIp, indexPort);
    
    vector<PeerFileDTO<ll>> list = fileDirectoryReader(stoi(port), ip, directory);
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
