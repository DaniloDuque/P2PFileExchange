    
#include "PeerServer.h"

int main(int argc, char const *argv[]) {

    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <mainPort> <indexIp> <indexPort>" << std::endl;
        return -1;
    }

    std::string mainPort = argv[1];
    std::string indexIp = argv[2];
    std::string indexPort = argv[3];

    vector<PeerFileDTO<long long>> list;
    std::string directory = "FileReader/testDirectory";
    
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }
    
    struct hostent *server = gethostbyname(indexIp.c_str());
    if (server == NULL) {
        std::cerr << "Error: could not resolve server address" << std::endl;
        close(clientSocket);
        return -1;
    }
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(std::stoi(indexPort));  
    memcpy(&serverAddress.sin_addr.s_addr, server->h_addr_list[0], server->h_length);  
    
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error connecting to the server" << std::endl;
        close(clientSocket);
        return -1;
    }
    
    list = fileDirectoryReader(directory);
    std::string package = "1 ";
    for (int i = 0; i < (int)list.size(); i++) {
        package += list[i].serialize();
        if (i != (int)list.size() - 1) {
            package += ' ';
        }
    }
    
    if (send(clientSocket, package.c_str(), package.size(), 0) < 0) {
        std::cerr << "Error sending the package" << std::endl;
        close(clientSocket);
        return -1;
    }

    puts("Package sent!");
    close(clientSocket);
    return 0;
}
