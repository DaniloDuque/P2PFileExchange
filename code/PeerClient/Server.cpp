#include "PeerServer.h"





int main(int argc, char const * argv[]) {

    
    std::string mainPort = argv[1];
    std::string indexIp = argv[2];
    std::string indexPort = argv[3];

    vector<PeerFileDTO<long long>> list;

    std::string directory = "FileReader/testDirectory";

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    struct hostent *server = gethostbyname(indexIp.c_str());
    memcpy(&serverAddress.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));


    list = fileDirectoryReader(directory);
    std::string package = "1 ";
    for(int i = 0; i < list.size(); i++) {
        package += list[i].serialize();
        if(i != list.size() - 1) {
            package += ' ';
        }

    }

    send(clientSocket, package.c_str(), package.size(), 0);

    std::cout << package << endl;



    return 0;
}
