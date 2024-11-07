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
    string finfo = getFileInfo(indexSocket, filename);
    if(finfo.empty()) {cerr<<"Bad Response from index"<<endl; return -1;}
    if(finfo[0]!='0') {cerr<<"Requested file not found in the network"<<endl; return 0;}
    FileInfo<ll> info = FileInfo<ll>::deserialize(finfo.substr(1, finfo.size()));
    cout<<finfo<<endl;
    return 0;
}
