#pragma once
#include "Orchestrator.h"
#include "client/TCPClient.cpp"
#include "server/PeerServer.cpp"
#include "encoder/Base64Encoder.cpp"
#include "common/bytestream/TCPStream.cpp"

Orchestrator::Orchestrator(const string& local_ip, const int server_port,
                           const int client_port, const string& index_ip,
                           const int index_port, const string& shared_directory,
                           const string& output_directory) {
    const auto peer_desc = PeerDescriptor(local_ip, client_port);
    const auto server_desc = PeerDescriptor(local_ip, server_port);
    const auto index_desc = PeerDescriptor(index_ip, index_port);
    shared_files = indexer.index_files(shared_directory);

    const auto stream = make_shared<TCPStream>();
    const auto encoder = make_shared<Base64Encoder>();

    client = make_shared<TCPClient>(stream, encoder, peer_desc, server_desc, index_desc, output_directory);
    server = make_shared<PeerServer>(encoder, stream, shared_files, shared_directory, server_port);
}

void Orchestrator::stop() const {
    logger.info("Stopping peer orchestrator");
    client->remove_peer();
    server->stop();
}

void Orchestrator::start() const {
    logger.info("Starting peer orchestrator");
    thread server_thread([this] { server->run(); });
    server_thread.detach();
    client->add_peer(shared_files);
}

pair<bool, SearchResult> Orchestrator::search_file(const string& filename) const {
    return client->search_file(filename);
}

shared_ptr<FileInfo> Orchestrator::request_file(const FileDescriptor& descriptor) const {
    return client->request_file(descriptor);
}

bool Orchestrator::download_file(const FileInfo& file, const string& name) const {
    return client->download_file(file, name);
}

