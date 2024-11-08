#include "PeerServer.h"

template<typename T>
PeerServer<T>::PeerServer(int p, string ip, string directory) : TCPServer(p) {
    vector<PeerFileDTO<T>> list = fileDirectoryReader(p, ip, directory);
    for(auto &pf : list) HashedFiles.emplace(pf.h1, pf.h2, pf.size, pf.alias);
}

template<typename T>
int PeerServer<T>::connectToIndex(string indexIp, string indexPort){

    int indexSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (indexSocket < 0) {
        cerr << "Error creating socket" << endl;
        return -1;
    }

    struct hostent *server = gethostbyname(indexIp.c_str());
    if (server == NULL) {
        cerr << "Error: could not resolve server address" << endl;
        close(indexSocket);
        return -1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(stoi(indexPort));  
    memcpy(&serverAddress.sin_addr.s_addr, server->h_addr_list[0], server->h_length);  

    if (connect(indexSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error connecting to the server" << endl;
        close(indexSocket);
        return -1;
    }

    return indexSocket;

}

template<typename T>
string PeerServer<T>::searchFile(FileRequestDTO<T> dto){
    auto it = HashedFiles.find(HashedFile<T>{dto.h1, dto.h2, dto.size, ""});
    if(it == HashedFiles.end()) return "";  
    return it->alias;  
}

template<typename T>
void PeerServer<T>::handleClient(int peerSocket) {
    string rqst = readBuffer(peerSocket, BUFFER_SIZE);
    if(rqst.size()) sendFilePart(peerSocket, FileRequestDTO<T>::deserialize(rqst));
    readBuffer(peerSocket, 1); 
    close(peerSocket); 
}

template<typename T>
void PeerServer<T>::sendFilePart(int peerSocket, FileRequestDTO<T> rqst){
    FILE* file = fopen(searchFile(rqst).c_str(), "rb");
    fseek(file, rqst.startByte, SEEK_SET);
    char buffer[BUFFER_SIZE];
    T leftBytes = rqst.chunkSize;
    while (leftBytes > 0) {
        size_t bytesToRead = min(leftBytes, static_cast<T>(BUFFER_SIZE));
        size_t bytesRead = fread(buffer, 1, bytesToRead, file);
        if (bytesRead > 0) {
            send(peerSocket, buffer, bytesRead, 0); 
            leftBytes -= bytesRead;
        } else {
            cerr << "Error reading from file or end of file reached." << endl;
            break;
        }
    }fclose(file); 
}
