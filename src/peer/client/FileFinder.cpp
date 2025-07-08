#include "util.h"
#include "peer/server/PeerServer.cpp"
#include "FileInfo.cpp"
#include "logger/Logger.h"
#include "constants.h"

pair<string, string> requestChosenFile(const int indexSocket, const string &fileName){
    const string package = to_string(FILE_REQUEST) + " " + fileName;
    if (send(indexSocket, package.c_str(), package.size(), 0) < 0) {
        logger.error("Error sending the package");
        close(indexSocket);
        return {"", ""};
    }
    logger.info("File info requested!");
    const string info = readSingleBuffer(indexSocket);
    if(info == ERR) return {ERR, ""};
    printFileInfoTable(info.substr(2, info.size()));
    string name;
    ll size;
    cout<<"Insert file name and file size: ";
    cin>>name>>size;
    cout<<endl;
    const string rsp = name + ' ' + to_string(size);
    sendBytes(indexSocket, rsp);
    string result = readSingleBuffer(indexSocket);
    if(result.empty() || result == ERR) return {ERR, ""};
    sendAcknowledge(indexSocket);
    return {result, name};
}

pair<string, string> getFileInfo(const int indexSocket, const string &fileName){
    pair<string, string> info = requestChosenFile(indexSocket, fileName);
    close(indexSocket);
    return info;
}

void requestFileChunk(const DownloadFileDTO &fileInfo, const PeerFileInfoDTO& peerInfo, const string& name) {
    const int peerSocket = PeerServer::connectToServer(peerInfo.ip, to_string(peerInfo.port));
    const string finfo = fileInfo.serialize();
    sendBytes(peerSocket, finfo);
    FILE* file = fopen(name.c_str(), "wb");
    ll bytesRead = 0;
    char buffer[BUFFER_SIZE];  
    while (bytesRead < fileInfo.chunkSize) {
        logger.info("Bytes read: " + to_string(bytesRead));
        const ll bytesToReceive = min(fileInfo.chunkSize - bytesRead, static_cast<ll>(BUFFER_SIZE));
        const ssize_t bytesReceived = read(peerSocket, buffer, bytesToReceive);
        if (bytesReceived < 0) {
            logger.error("Error receiving data from socket: " + string(strerror(errno)));
            fclose(file);
            close(peerSocket);
            return;
        }
        if (bytesReceived == 0) {
            logger.error("Connection closed by peer");
            break;
        }
        fwrite(buffer, 1, bytesReceived, file);
        bytesRead += bytesReceived;
    }
    fclose(file); close(peerSocket);
}

void requestFile(FileInfo &fileInfo, string &dir) {
    const ll numPeers = fileInfo.getNumberOfPeersWithFile();
    const ll chunkSize = fileInfo.getSize() / numPeers;
    ll startByte = 0, i = 0, mod = fileInfo.getSize() % numPeers;
    vector<thread> threads;
    for(auto &pfi : fileInfo.getFileInfo()){
        const ll hash1 = fileInfo.getHash1();
        const ll hash2 = fileInfo.getHash2();
        const ll size = fileInfo.getSize();
        threads.emplace_back([=, &dir] {
            requestFileChunk(DownloadFileDTO{hash1, hash2, size, startByte, chunkSize + (mod > 0)}, pfi, dir+"/"+to_string(i));
        });
        i++; startByte += chunkSize + (mod > 0); mod--;
    }
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
}

void downloadFile(FileInfo &fileInfo, string &directory, const string &fileName) {
    requestFile(fileInfo, directory);
    const filesystem::path sandbox{directory};
    FILE* file = fopen(fileName.c_str(), "wb");
    vector<filesystem::path> files;
    for (auto& dirEntry : filesystem::directory_iterator{sandbox}) files.push_back(dirEntry.path());
    ranges::sort(files);
    for (const auto& filePath : files) {
        FILE* cub = fopen(filePath.c_str(), "rb");
        if (!cub) {
            logger.error("Error opening chunk file: " + string(filePath));
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
        logger.error("Usage: " + string(argv[0]) + " <mainPort> <indexIp> <indexPort> <directory>");
        return -1;
    }
    string mainPort=argv[1], indexIp=argv[2], indexPort=argv[3], fileName=argv[4], directory=argv[5];
    int indexSocket = PeerServer::connectToServer(indexIp, indexPort);
    auto [finfo, actualFileName] = getFileInfo(indexSocket, fileName);
    if(finfo.empty()) { logger.error("Bad Response from index"); return -1; }
    if(to_string(finfo[0]) != OK) { logger.error("Requested file not found in the network"); return 0; }
    auto info = FileInfo::deserialize(finfo.substr(2, finfo.size()));
    downloadFile(*info, directory, actualFileName);
}