#include "../../util.h"
#include "../FileExchange/PeerServer.h"
#include "FileInfo.h"

pair<string, string> requestChosenFile(int indexSocket, string &fileName){
    string package = "2 " + fileName;
    if (send(indexSocket, package.c_str(), package.size(), 0) < 0) {
        cerr << "Error sending the package" << endl;
        close(indexSocket);
        return {"", ""};
    }
    puts("File info requested!");
    string info = readSingleBuffer(indexSocket);
    if(info=="1") return {"1", ""};
    printFileInfoTable(info.substr(2, info.size()));
    string name;
    ll size;
    cout<<"Insert file name and file size: ";
    cin>>name>>size;
    cout<<endl;
    string rsp = name + ' ' + to_string(size);
    sendBytes(indexSocket, rsp);
    string rslt = readSingleBuffer(indexSocket);
    if(rslt.empty() || rslt == "1") return {"1", ""};
    sendAcknowledge(indexSocket);
    return {rslt, name};
}

pair<string, string> getFileInfo(int indexSocket, string &fileName){
    pair<string, string> info = requestChosenFile(indexSocket, fileName);
    close(indexSocket);
    return info;
}

void requestFileChunk(FileRequestDTO<ll> fileInfo, PeerFileInfo peerInfo, string name) {
    int peerSocket = PeerServer<ll>::connectToServer(peerInfo.ip, to_string(peerInfo.port));
    string finfo = fileInfo.serialize();
    sendBytes(peerSocket, finfo);
    FILE* file = fopen(name.c_str(), "wb");
    ll bytesRead = 0;
    char buffer[BUFFER_SIZE];  
    while (bytesRead < fileInfo.chunkSize) {
        cout << "Bytes read: " << bytesRead << endl;
        size_t bytesToReceive = min(fileInfo.chunkSize - bytesRead, static_cast<ll>(BUFFER_SIZE));
        ssize_t bytesReceived = read(peerSocket, buffer, bytesToReceive);
        if (bytesReceived < 0) {
            cerr << "Error receiving data from socket: " << strerror(errno) << endl;
            fclose(file);
            close(peerSocket);
            return;
        }
        if (bytesReceived == 0) {
            cerr << "Connection closed by peer." << endl;
            break;
        }
        fwrite(buffer, 1, bytesReceived, file);
        string ack = "ACK";
        sendBytes(peerSocket, ack);
        bytesRead += bytesReceived;
    }
    fclose(file); close(peerSocket);
}

void requestFile(FileInfo<ll> fileInfo, string &dir) {
    ll numPeers = fileInfo.getNumberOfPeersWithFile();
    ll chunkSize = fileInfo.getSize() / numPeers;
    ll startByte = 0, i = 0, mod = fileInfo.getSize() % numPeers;
    vector<thread> threads;
    for(auto &pfi : fileInfo.getFileInfo()){
        threads.emplace_back([&, i, startByte, chunkSize, mod]() {
            requestFileChunk(FileRequestDTO<ll>{fileInfo.getHash1(), fileInfo.getHash2(), fileInfo.getSize(), startByte, chunkSize + (mod > 0)}, pfi, dir+"/"+to_string(i));
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
    for (const auto& filePath : files) {
        FILE* cub = fopen(filePath.c_str(), "rb");
        if (!cub) {
            cerr << "Error opening chunk file: " << filePath << endl;
            fclose(file);
            return;
        }
        char buffer[BUFFER_SIZE];
        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), cub)) > 0) {
            fwrite(buffer, 1, bytesRead, file);
        }
        fclose(cub);
    }
    fclose(file);
    for (const auto& filePath : files) {
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
    auto [finfo, actualFileName] = getFileInfo(indexSocket, fileName);
    if(finfo.empty()) {cerr<<"Bad Response from index"<<endl; return -1;}
    if(finfo[0]!='0') {cerr<<"Requested file not found in the network"<<endl; return 0;}
    FileInfo<ll> info = FileInfo<ll>::deserialize(finfo.substr(2, finfo.size()));
    downloadFile(info, directory, actualFileName);
    return 0;
}
