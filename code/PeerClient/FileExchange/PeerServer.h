#ifndef PEER_SERVER_HEADER
#define PEER_SERVER_HEADER

#include "../../util.h"
#include "../../Network/TCPServer.h"
#include "FileRequestDTO.h"
#include "../FileReader/fileReader.cpp"

template<typename T>
class PeerServer : public TCPServer {

private:

    string path;
    set<File<T>> sharedFiles;
    void handleClient(int) override;
    void sendFilePart(int, FileRequestDTO<T>);
    string searchFile(FileRequestDTO<T>);

public:

    static int connectToServer(string, string);
    PeerServer(int, string&);

};

#include "PeerServer.cpp"

#endif
