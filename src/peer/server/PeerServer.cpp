#pragma once
#include "util.h"
#include "common/server/TCPServer.cpp"
#include "dto/DownloadFileChunkDTO.cpp"
#include "logger/Logger.h"

class PeerServer final : public TCPServer {
    const shared_ptr<Encoder> encoder;
    const set<IndexedFileDescriptor> shared_files;
    const string path;

    void handle_client(const int peerSocket) override {
        const auto [status, payload] = stream->read(peerSocket);
        logger.info("Client on socket: " + to_string(peerSocket));
        if(status && !payload.empty()) handle_file_chunk_request(peerSocket, DownloadFileChunkDTO::deserialize(payload));
        close(peerSocket);
    }

    void handle_file_chunk_request(const int peerSocket, const DownloadFileChunkDTO &request) const {
        const string filePath = path + "/" + search_file(request);
        FILE* file = fopen(filePath.c_str(), "rb");
        if (!file) {
            logger.error("Error opening file: " + filePath);
            return;
        }
        fseek(file, request.startByte, SEEK_SET);
        char buffer[request.chunkSize] = {};
        fread(buffer, 1, request.chunkSize, file);
        const string file_chunk = encoder->encode(string(buffer, request.chunkSize));
        stream->write(true, peerSocket, file_chunk);
        fclose(file);
    }

    string search_file(const DownloadFileChunkDTO &dto) const {
        if(const auto it = shared_files.find(dto.file); it != shared_files.end())
            return it->filename;
        return "";
    }

public:
    PeerServer(const shared_ptr<Encoder>& encoder,
               const shared_ptr<ByteStream>& stream,
               const set<IndexedFileDescriptor>& shared_files,
               const string& path,
               const int port) : TCPServer(stream, port), encoder(encoder), shared_files(shared_files), path(path) {}

};

