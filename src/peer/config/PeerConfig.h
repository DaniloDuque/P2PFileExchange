#pragma once
#include <string>
#include <memory>
#include "common/descriptor/PeerDescriptor.cpp"
#include "common/descriptor/IndexedFileDescriptor.cpp"
#include "client/Client.h"
#include "common/server/Server.h"
#include <common/bytestream/TCPStream.cpp>
#include <encoder/Base64Encoder.cpp>
#include <client/TCPClient.cpp>
#include <server/PeerServer.cpp>
#include <set>
using namespace std;

struct PeerConfig {
    const PeerDescriptor peer;
    const PeerDescriptor server;
    const PeerDescriptor index;
    const string sharedDirectory;

    PeerConfig(const string& local_ip, const int server_port,
               const int client_port, const string& index_ip,
               const int index_port, const string& shared_dir)
    : peer(local_ip, client_port), server(local_ip, server_port), index(index_ip, index_port), sharedDirectory(shared_dir) {}

    shared_ptr<Client> createClient() const {
        shared_ptr<ByteStream> stream = make_shared<TCPStream>();
        shared_ptr<Encoder> encoder = make_shared<Base64Encoder>();
        return make_shared<TCPClient>(stream, encoder, peer, server, index, sharedDirectory);
    }

    shared_ptr<Server> createServer(const set<IndexedFileDescriptor>& sharedFiles) const {
        shared_ptr<ByteStream> stream = make_shared<TCPStream>();
        shared_ptr<Encoder> encoder = make_shared<Base64Encoder>();
        return make_shared<PeerServer>(encoder, stream, sharedFiles, sharedDirectory, server.port);
    }
};