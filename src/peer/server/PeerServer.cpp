#pragma once
#include <util.h>
#include <common/server/TCPServer.cpp>
#include <dto/DownloadFileChunkDTO.cpp>
#include <logger/Logger.h>
#include <encoder/Encoder.h>
#include <common/descriptor/IndexedFileDescriptor.cpp>

class PeerServer final : public TCPServer {
    const shared_ptr<Encoder> encoder;
    const set<IndexedFileDescriptor> shared_files;
    const string path;

    void handle_client(const int peerSocket) override {
        const auto [status, payload] = stream->read(peerSocket);
        logger.info("Client on socket: " + to_string(peerSocket));
        if (status && !payload.empty())
            handle_file_chunk_request(
                peerSocket, DownloadFileChunkDTO::deserialize(payload));
        close(peerSocket);
    }

    void handle_file_chunk_request(const int peerSocket, const DownloadFileChunkDTO &request) const {
        const string filename = search_file(request);
        if (filename.empty()) {
            logger.error("Requested file not found in shared files");
            stream->write(false, peerSocket, "File not found");
            return;
        }
        
        const string filePath = path + "/" + filename;
        FILE *file = fopen(filePath.c_str(), "rb");
        if (!file) {
            logger.error("Error opening file: " + filePath + " - " + strerror(errno));
            stream->write(false, peerSocket, "Cannot open file");
            return;
        }
        
        if (fseek(file, request.start_byte, SEEK_SET) != 0) {
            logger.error("Error seeking in file: " + filePath);
            stream->write(false, peerSocket, "Seek error");
            fclose(file);
            return;
        }
        
        if (request.chunk_size == 0) {
            logger.error("Invalid chunk size: 0");
            stream->write(false, peerSocket, "Invalid chunk size");
            fclose(file);
            return;
        }
        
        vector<unsigned char> buffer(request.chunk_size);
        const size_t bytes_read = fread(buffer.data(), 1, request.chunk_size, file);
        
        if (ferror(file)) {
            logger.error("Error reading file: " + filePath);
            stream->write(false, peerSocket, "Read error");
            fclose(file);
            return;
        }
        
        if (bytes_read == 0) {
            logger.warn("No bytes read from file: " + filePath);
            stream->write(false, peerSocket, "No data");
            fclose(file);
            return;
        }
        
        const string binary_data(buffer.begin(), buffer.begin() + bytes_read);
        const string file_chunk = encoder->encode(binary_data);
        
        if (!stream->write(true, peerSocket, file_chunk)) {
            logger.error("Failed to send file chunk");
        }
        
        fclose(file);
    }

    string search_file(const DownloadFileChunkDTO &dto) const {
        if (const auto it = shared_files.find(dto.file); it != shared_files.end())
            return it->filename;
        return "";
    }

public:
    PeerServer(const shared_ptr<Encoder> &encoder,
               const shared_ptr<ByteStream> &stream,
               const set<IndexedFileDescriptor> &shared_files,
               const string &path,
               const int port) : TCPServer(stream, port), encoder(encoder), shared_files(shared_files), path(path) {
    }
};
