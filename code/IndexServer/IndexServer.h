#ifndef INDEX_SERVER_HEADER
#define INDEX_SERVER_HEADER

#include "../util.h"
#include "../Network/TCPServer.h"
#include "FileMapping/FileIndex.h"
#include "FileMapping/FileInfo.h"
#include "FileMapping/NewPeerDTO.h"

template<typename T>
class IndexServer : public TCPServer {
private:
    FileIndex<T> index;
    void addPeer(NewPeerDTO<T>);
    vector<pair<string, FileInfo<T>*>> findMatches(string);
    string getFileMatchesResponse(vector<pair<string, FileInfo<T>*>>);
    FileInfo<T>* searchFileInMatches(vector<pair<string, FileInfo<T>*>>, string&, T);
    void handleClient(int) override;
    void handleAddPeer(string);
    void handleFileRequest(string, int);
public: 
    IndexServer(int);
};

#include "IndexServer.cpp"

#endif 
