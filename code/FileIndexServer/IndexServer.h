#ifndef INDEX_SERVER_HEADER
#define INDEX_SERVER_HEADER

#include "../util.h"
#include "../Network/TCPServer.h"
#include "FileMapping/FileIndex.h"
#include "FileMapping/FileInfo.h"
#include "PeerFileDTO.h"

template<typename T>
class IndexServer : public TCPServer {
private:
    FileIndex<T> index;
    void addPeer(vector<PeerFileDTO<T>>);
    FileInfo<T> findFile(string);
public: 
    void handleClient() override;
};

#include "IndexServer.cpp"

#endif 
