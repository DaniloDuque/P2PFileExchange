#include "PeerServer.h"

template<typename T>
PeerServer<T>::PeerServer(int p, string ip, string directory) : TCPServer(p), path(directory) {
    vector<PeerFileDTO<T>> list = fileDirectoryReader(p, ip, directory);
    for(auto &pf : list) HashedFiles.emplace(pf.h1, pf.h2, pf.size, pf.alias);
}

template<typename T>
int PeerServer<T>::connectToServer(string serverIp, string serverPort){

    cout<<serverIp<<' '<<serverPort<<endl;

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Error creating socket" << endl;
        return -1;
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
    auto it = HashedFiles.find(HashedFile<T>{dto.h1, dto.h2, dto.size, ""});
    if(it == HashedFiles.end()) return "";  
    return it->alias;  
}

template<typename T>
void PeerServer<T>::handleClient(int peerSocket) {
    cerr <<"Hello from peer server" << endl;
    string rqst = readBuffer(peerSocket, BUFFER_SIZE);
    if(rqst.size()) sendFilePart(peerSocket, FileRequestDTO<T>::deserialize(rqst));
    readBuffer(peerSocket, 1); 
    close(peerSocket); 
}

template<typename T>
void PeerServer<T>::sendFilePart(int peerSocket, FileRequestDTO<T> rqst){
    string filePath = path + "/" + searchFile(rqst);
    FILE* file = fopen(filePath.c_str(), "rb");
    cout<<rqst.startByte<<' '<<rqst.chunkSize<<endl;
    fseek(file, rqst.startByte, SEEK_SET);
    char buffer[BUFFER_SIZE];
    T leftBytes = rqst.chunkSize;
    while (leftBytes > 0) {
	cout<<leftBytes<<endl;
        size_t bytesToRead = min(leftBytes, static_cast<T>(BUFFER_SIZE));
        size_t bytesRead = fread(buffer, 1, bytesToRead, file);
        if (bytesRead > 0) {
            send(peerSocket, buffer, bytesRead, 0); 
            leftBytes -= bytesRead;
        } else {
            cerr << "End of file reached." << endl;
            break;
        }
        receiveAcknowledge(peerSocket);
    }
    fclose(file); 
}
