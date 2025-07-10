#pragma once
#include <string>
#include "common/descriptor/PeerDescriptor.cpp"
#include "common/descriptor/FileDescriptor.cpp"
#include "common/descriptor/SearchResult.cpp"
#include "common/bytestream/ByteStream.h"
#include "encoder/Encoder.h"
#include "fileinfo/FileInfo.cpp"
using namespace std;

class Client {
protected:
    mutable mutex write_file_mutex;
    shared_ptr<ByteStream> stream;
    shared_ptr<Encoder> encoder;
    const PeerDescriptor peer, index;
    const string shared_directory;

    virtual int connect_to_server(const PeerDescriptor&) = 0;

public:
    Client() = delete;

    Client(shared_ptr<ByteStream> stream,
           shared_ptr<Encoder> encoder,
           const PeerDescriptor& peer,
           const PeerDescriptor& index,
           const string& shared_directory)
        : stream(move(stream)), encoder(move(encoder)), peer(peer), index(index), shared_directory(shared_directory) {}

    virtual ~Client() = default;

    virtual bool add_peer() = 0;
    virtual bool remove_peer() = 0;
    virtual pair<bool, SearchResult> search_file(const string&) = 0;
    virtual pair<bool, FileInfo> request_file(const FileDescriptor&) = 0;
    virtual bool download_file(const FileInfo&, const string&) = 0;
};
