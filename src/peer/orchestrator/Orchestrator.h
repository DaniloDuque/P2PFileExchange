#pragma once
#include <memory>

#include "Client.h"
#include "indexer/FileIndexer.h"
#include "server/Server.h"

class Orchestrator {
    const shared_ptr<Client> client;
    const shared_ptr<Server> server;
    const FileIndexer indexer;



};
