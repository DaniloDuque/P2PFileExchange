#include "FileExchange/PeerServer.h"

int main(int argc, char const *argv[]) {

    if (argc < 6) {
        cerr << "Usage: " << argv[0] << " <port> <indexIp> <indexPort>" << endl;
        return -1;
    }

    string port = argv[1];
    string ip = argv[2];
    string indexIp = argv[3];
    string indexPort = argv[4];
    string directory = argv[5];

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Error creating socket" << endl;
        return -1;
    }
    
    struct hostent *server = gethostbyname(indexIp.c_str());
    if (server == NULL) {
        cerr << "Error: could not resolve server address" << endl;
        close(clientSocket);
        return -1;
    }
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(stoi(indexPort));  
    memcpy(&serverAddress.sin_addr.s_addr, server->h_addr_list[0], server->h_length);  
    
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error connecting to the server" << endl;
        close(clientSocket);
        return -1;
    }
    
    vector<PeerFileDTO<long long>> list = fileDirectoryReader(stoi(port), ip, directory);
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
        return -1;
    }

    puts("Package sent!");
    close(clientSocket);
    return 0;
}
