#ifndef PEER_SERVER_HEADER
#define PEER_SERVER_HEADER

#include "../util.h"
#include "../Network/TCPServer.h"
#include "FileExchange/FileRequestDTO.h"
#include "FileExchange/HashedFile.h"
#include "FileReader/fileReader.cpp"

template<typename T>
class PeerServer : public TCPServer {

private:

    set<HashedFile<T>> HashedFiles;
    void handleClient(int) override;
    void sendFilePart(int, FileRequestDTO<T>);
    string searchFile(FileRequestDTO<T>);

public:

    static int connectToIndex(string&, string&);
    PeerServer(int, string);

};

#include "PeerServer.cpp"

#endif
