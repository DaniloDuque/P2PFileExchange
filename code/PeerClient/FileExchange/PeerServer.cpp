#include "PeerServer.h"

template<typename T>
PeerServer<T>::PeerServer(int p, string &directory) : TCPServer(p), path(directory) {
    vector<File<T>> list = fileDirectoryReader<T>(directory);
    for(auto &pf : list) sharedFiles.emplace(pf.hash1, pf.hash2, pf.size, pf.fileName);
}

template<typename T>
int PeerServer<T>::connectToServer(string serverIp, string serverPort){

    cout<<"Connecting to server at: "<<serverIp<<" (Port: "<<serverPort<<")"<<endl;

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Error creating socket" << endl;
        return -1;
    }

    T bufferSize = BUFFER_SIZE;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) < 0) {
        cerr << "Error setting receive buffer size: " << strerror(errno) << endl;
    }
    if (setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize)) < 0) {
        cerr << "Error setting send buffer size: " << strerror(errno) << endl;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(stoi(serverPort));

    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr) <= 0) {
        cerr << "Error: Invalid address/ Address not supported" << endl;
        close(serverSocket);
        return -1;
    }

    if (connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error connecting to the server" << endl;
        close(serverSocket);
        return -1;
    }

    return serverSocket;
}

template<typename T>
string PeerServer<T>::searchFile(FileRequestDTO<T> dto){
    auto it = sharedFiles.find(File<T>{dto.hash1, dto.hash2, dto.size, ""});
    if(it == sharedFiles.end()) return "";  
    return it->fileName;  
}

template<typename T>
void PeerServer<T>::handleClient(int peerSocket) {
    string rqst = readSingleBuffer(peerSocket);
    cout<<"Sharing file on socket: "<<peerSocket<<endl;
    if(rqst.size()) sendFilePart(peerSocket, FileRequestDTO<T>::deserialize(rqst));
    receiveAcknowledge(peerSocket); 
    close(peerSocket); 
}

template<typename T>
void PeerServer<T>::sendFilePart(int peerSocket, FileRequestDTO<T> rqst) {
    std::string filePath = path + "/" + searchFile(rqst);
    FILE* file = fopen(filePath.c_str(), "rb");
    if (!file) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }
    fseek(file, rqst.startByte, SEEK_SET);
    char buffer[BUFFER_SIZE];
    T leftBytes = rqst.chunkSize;
    while (leftBytes > 0) {
        size_t bytesToRead = std::min(leftBytes, static_cast<T>(BUFFER_SIZE));
        size_t bytesRead = fread(buffer, 1, bytesToRead, file);
        if (bytesRead > 0) {
            std::string data(buffer, bytesRead);
            sendBytes(peerSocket, data);
            leftBytes -= bytesRead;
            string ack = readBytes(peerSocket, 3); 
            if (ack != "ACK") {
                std::cerr << "Error receiving ACK from client." << std::endl;
                break;
            }
        } else {
            std::cerr << "Error reading from file or end of file." << std::endl;
            break;
        }
    }fclose(file);
}
