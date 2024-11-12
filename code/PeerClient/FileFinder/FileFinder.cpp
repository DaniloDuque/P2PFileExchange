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
    string info = readSingleBuffer(indexSocket);
    sendAcknowledge(indexSocket);
    close(indexSocket);
    return info;
}

void requestFileChunk(FileRequestDTO<ll> fileInfo, PeerInfo peerInfo, string name) {
    int peerSocket = PeerServer<ll>::connectToServer(peerInfo.ip, to_string(peerInfo.port));
    string finfo = fileInfo.serialize();
    sendBytes(peerSocket, finfo);
    FILE* file = fopen(name.c_str(), "wb");
    ll bytesRead = 0;
    while (bytesRead < fileInfo.chunkSize) {
	    cout<<bytesRead<<endl;
        size_t bytesToReceive = min(fileInfo.chunkSize-bytesRead, static_cast<ll>(BUFFER_SIZE));
        string buffer = readBytes(peerSocket, bytesToReceive);
    	bytesRead+=buffer.size();
        if(buffer.empty()) {cerr << "Error receiving data from socket." << endl; return;}
        fwrite(buffer.c_str(), 1, buffer.size(), file);
    }fclose(file);
    close(peerSocket);
}

void requestFile(FileInfo<ll> fileInfo, string &dir) {
    ll numPeers = fileInfo.addr.size();
    ll chunkSize = fileInfo.size / numPeers;
    ll startByte = 0, i = 0, mod = fileInfo.size % numPeers;
    vector<thread> threads;
    while (i < numPeers) {
        threads.emplace_back([&, i, startByte, chunkSize, mod]() {
            requestFileChunk(FileRequestDTO<ll>{fileInfo.hash1, fileInfo.hash2, fileInfo.size, startByte, chunkSize + (mod > 0)}, fileInfo.addr[i], dir+"/"+to_string(i));
        });
        i++; startByte += chunkSize + (mod > 0); mod--;
    }
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
}

void downloadFile(FileInfo<ll> fileInfo, string directory, string fileName) {
    requestFile(fileInfo, directory);
    const filesystem::path sandbox{directory};
    FILE* file = fopen(fileName.c_str(), "wb");
    vector<filesystem::path> files;
    for (auto& dirEntry : filesystem::directory_iterator{sandbox}) files.push_back(dirEntry.path());
    sort(files.begin(), files.end());
    for(auto const & dirEntry : std::filesystem::directory_iterator{sandbox}) {
        FILE * cub = fopen(dirEntry.path().c_str(), "rb");
        FILE * cubCopy = fopen(dirEntry.path().c_str(), "rb");
        char chC = fgetc(cubCopy);
        chC = fgetc(cubCopy);
        char ch = fgetc(cub);
        while(chC != EOF) {
            fprintf(file, "%c", ch);
            ch = fgetc(cub);
            chC = fgetc(cubCopy);
        }
        fclose(cub); fclose(cubCopy);
    }fclose(file);
    for (auto& filePath : files) {
        if (filePath.filename() != "0") filesystem::remove(filePath);
    }
}

int main(int argc, char const *argv[]) {
    if (argc < 6) {  
        cerr << "Usage: " << argv[0] << " <mainPort> <indexIp> <indexPort> <directory>" << endl;
        return -1;
    }
    string mainPort=argv[1], indexIp=argv[2], indexPort=argv[3], fileName=argv[4], directory=argv[5];
    int indexSocket = PeerServer<ll>::connectToServer(indexIp, indexPort);
    string finfo = getFileInfo(indexSocket, fileName);
    if(finfo.empty()) {cerr<<"Bad Response from index"<<endl; return -1;}
    if(finfo[0]!='0') {cerr<<"Requested file not found in the network"<<endl; return 0;}
    FileInfo<ll> info = FileInfo<ll>::deserialize(finfo.substr(1, finfo.size()));
    downloadFile(info, directory, fileName);
    return 0;
}
