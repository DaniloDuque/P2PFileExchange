#pragma once
#include <memory>
#include <thread>
#include "client/Client.h"
#include "indexer/FileIndexer.h"
#include "common/server/Server.h"
#include "common/descriptor/IndexedFileDescriptor.cpp"
#include "config/PeerConfig.h"
using namespace std;

class Orchestrator {
    shared_ptr<Client> client;
    shared_ptr<Server> server;
    FileIndexer indexer;
    set<IndexedFileDescriptor> sharedFiles;
    
public:
    Orchestrator(const PeerConfig& config);
    ~Orchestrator();
    
    void start();
    void stop();
    
};
