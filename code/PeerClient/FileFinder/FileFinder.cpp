#include "../../util.h"
#include "../FileExchange/PeerServer.h"
#include "FileInfo.h"

string getFileInfo(int indexSocket, string &filename){
    string package = "2 " + filename;
    if (send(indexSocket, package.c_str(), package.size(), 0) < 0) {
        cerr << "Error sending the package" << endl;
        close(indexSocket);
        return "";
    }
    puts("File info requested!");
    string info = readBuffer(indexSocket, BUFFER_SIZE);
    package = "0";
    send(indexSocket, package.c_str(), package.size(), 0);
    close(indexSocket);
    return info;
}

int main(int argc, char const *argv[]) {
    if (argc < 5) {  
        cerr << "Usage: " << argv[0] << " <mainPort> <indexIp> <indexPort> <directory>" << endl;
        return -1;
    }
    string mainPort = argv[1], indexIp = argv[2], indexPort = argv[3], filename = argv[4];
    int indexSocket = PeerServer<ll>::connectToIndex(indexIp, indexPort);
    FileInfo<ll> info = FileInfo<ll>::deserialize(getFileInfo(indexSocket, filename));
    // TODO: now having all the peers that have the file, request it to them
    return 0;
}
