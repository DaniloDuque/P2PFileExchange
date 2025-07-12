#pragma once
#include <memory>
#include <client/Client.h>
#include <indexer/FileIndexer.h>
#include <common/server/Server.h>
#include <common/descriptor/IndexedFileDescriptor.cpp>
using namespace std;

class Orchestrator {
    shared_ptr<Client> client;
    shared_ptr<Server> server;
    FileIndexer indexer;
    set<IndexedFileDescriptor> shared_files;

public:
    Orchestrator(const string &local_ip, int server_port,
                 int client_port, const string &index_ip,
                 int index_port, const string &shared_directory,
                 const string &output_directory);

    void start() const;

    void stop() const;

    pair<bool, SearchResult> search_file(const string &) const;

    shared_ptr<FileInfo> request_file(const FileDescriptor &) const;

    bool download_file(const FileInfo &, const string &) const;
};
