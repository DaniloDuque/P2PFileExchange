#ifndef INDEX_SERVER_HEADER
#define INDEX_SERVER_HEADER

#include "../util.h"
#include "../Network/TCPServer.h"
#include "FileMapping/FileIndex.h"
#include "FileMapping/PeerInfo.h"
#include "NewPeerDTO.h"
#include "PeerFileDTO.h"

template<typename T>
class IndexServer : public TCPServer {
private:
    FileIndex<T> index;
    void addPeer(NewPeerDTO<T>);
    vector<PeerInfo<T>*> findFiles(string);
    void handleClient(int) override;
    void handleAddPeer(string);
    void handleFindFile(string, int);
public: 
    IndexServer(int);
};

#include "IndexServer.cpp"

#endif 
