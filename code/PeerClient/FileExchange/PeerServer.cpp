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
    std::lock_guard<std::mutex> lock(fileMutex);
    
    std::string filePath = path + "/" + searchFile(rqst);
    FILE* file = fopen(filePath.c_str(), "rb");
    if (!file) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    if (fseek(file, rqst.startByte, SEEK_SET) != 0) {
        std::cerr << "Error seeking to position " << rqst.startByte << std::endl;
        fclose(file);
        return;
    }

    char buffer[BUFFER_SIZE];
    T leftBytes = rqst.chunkSize;
    bool error = false;

    while (leftBytes > 0 && !error) {
        size_t bytesToRead = std::min(leftBytes, static_cast<T>(BUFFER_SIZE));
        size_t bytesRead = fread(buffer, 1, bytesToRead, file);
        
        if (bytesRead > 0) {
            std::lock_guard<std::mutex> sockLock(socketMutex);
            if (send(peerSocket, buffer, bytesRead, MSG_NOSIGNAL) < 0) {
                std::cerr << "Error sending data: " << strerror(errno) << std::endl;
                error = true;
                break;
            }
            
            leftBytes -= bytesRead;
            
            struct timeval timeout;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            setsockopt(peerSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            
            char ackBuffer[4] = {0};
            ssize_t ackBytes = recv(peerSocket, ackBuffer, 3, 0);
            if (ackBytes != 3 || strncmp(ackBuffer, "ACK", 3) != 0) {
                std::cerr << "Error receiving ACK from client." << std::endl;
                error = true;
                break;
            }
        } else {
            if (ferror(file)) {
                std::cerr << "Error reading from file: " << strerror(errno) << std::endl;
            } else {
                std::cerr << "Unexpected end of file." << std::endl;
            }
            error = true;
            break;
        }
    }
    
    fclose(file);
}
