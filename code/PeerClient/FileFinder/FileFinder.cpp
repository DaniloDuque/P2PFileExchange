#include "../../util.h"
#include "../FileExchange/PeerServer.h"
#include "FileInfo.h"

mutex mtx;

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

// TODO: request the file part and save it to disk
void requestFileChunk(FileRequestDTO<ll> fileInfo, PeerInfo peerInfo, string name){
    mtx.lock();
    cout<<"FileRequestDTO: "<<fileInfo.serialize()<<endl;
    cout<<"PeerInfo: "<<peerInfo.serialize()<<endl;
    cout<<"name: "<<name<<endl;
    mtx.unlock();
}

void requestFile(FileInfo<ll> fileInfo) {
    ll numPeers = fileInfo.addr.size();
    ll chunkSize = fileInfo.size / numPeers;
    ll startByte = 0, i = 0, mod = fileInfo.size % numPeers;
    vector<thread> threads;
    while (i < numPeers) {
        threads.emplace_back([&, i, startByte, chunkSize, mod]() {
            requestFileChunk(FileRequestDTO<ll>{fileInfo.hash1, fileInfo.hash2, fileInfo.size, startByte, chunkSize + (mod > 0)}, 
                             fileInfo.addr[i], to_string(i));
        });
        i++; startByte += chunkSize + (mod > 0); mod--;
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
}

void downloadFile(FileInfo<ll> fileInfo){
    requestFile(fileInfo);
    // TODO: append all the file parts in order in a single file
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
    downloadFile(info);
    return 0;
}
