#pragma once
#include "Client.h"
#include "util.h"
#include "peer/server/PeerServer.cpp"
#include "../fileinfo/FileInfo.cpp"
#include "logger/Logger.h"
#include "constants.h"
#include "dto/AddPeerDTO.cpp"
#include "dto/RemovePeerDTO.cpp"
#include "dto/FileSearchDTO.cpp"
#include "dto/FileRequestDTO.cpp"
#include "dto/SearchResultDTO.cpp"
#include <fcntl.h>
#include <unistd.h>
using namespace std;

class TCPClient final : public Client {
protected:
    int connect_to_server(const PeerDescriptor& peer) override {
        logger.info("Establishing connection to " + peer.ip + ":" + to_string(peer.port));
        const int socket = socket(AF_INET, SOCK_STREAM, 0);
        if (socket < 0) {
            logger.error("Error creating socket");
            return -1;
        }
        constexpr
        size_t bufferSize = BUFFER_SIZE;
        if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize)) < 0)
            logger.warn("Error setting receive buffer size: " + string(strerror(errno)));

        if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize)) < 0)
            logger.warn("Error setting send buffer size: " + string(strerror(errno)));

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(peer.port);
        if (inet_pton(AF_INET, peer.ip.c_str(), &serverAddress.sin_addr) <= 0) {
            logger.error("Invalid address or address not supported");
            close(socket);
            return -1;
        }
        if (connect(socket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
            logger.error("Error connecting to the server: " + string(strerror(errno)));
            close(socket);
            return -1;
        }
        return socket;
    }

    template<typename T>
    pair<bool, T> deserialize_response(const string &response) {
        if (response.empty() || response[0]+"" != OK) return {false, T()};
        return {true, T::deserialize(response.substr(2))};
    }

    void download_file_chunk(const DownloadFileChunkDTO& dto, const PeerDescriptor& peer, const string& filename, const size_t offset) {
        const int peerSocket = connect_to_server(peer);
        const string finfo = dto.serialize();
        stream->write(peerSocket, finfo);
        const string file_chunk = stream->read(peerSocket);
        const string decoded_file_chunk = encoder->decode(file_chunk);

        const int fd = open(filename.c_str(), O_WRONLY);
        if (fd < 0) {
            logger.error("Error opening file for writing: " + filename);
            close(peerSocket);
            return;
        }

        if (const ssize_t bytesWritten = pwrite(fd, decoded_file_chunk.data(), decoded_file_chunk.size(), offset); bytesWritten < 0) {
            logger.error("Error in pwrite: " + string(strerror(errno)));
        }

        close(fd); close(peerSocket);
    }

public:
    TCPClient() = delete;
    TCPClient(const shared_ptr<ByteStream> &stream,
                  const shared_ptr<Encoder> &encoder,
                  const PeerDescriptor& peer,
                  const PeerDescriptor& index,
                  const string& shared_directory)
            : Client(stream, encoder, peer, index, shared_directory) {}

    ~TCPClient() override = default;

    bool add_peer() override {
        int clientSocket = 0;
        for(int i = 0; i < 5; i++) {
            clientSocket = connect_to_server(index);
            if(clientSocket >= 0) break;
            this_thread::sleep_for(chrono::milliseconds(200));
        }
        if (clientSocket < 0) {
            logger.error("Failed to connect to index server after retries");
            return false;
        }
        const vector<IndexedFileDescriptor> files = fileDirectoryReader(shared_directory);
        const AddPeerDTO dto(peer, files);
        if (const string package = to_string(ADD_PEER) + " " + dto.serialize(); !stream->write(clientSocket, package)) {
            logger.error("Error sending the package");
            close(clientSocket);
            return false;
        }
        logger.info("Package sent!");
        close(clientSocket);
        return true;
    }

    bool remove_peer() override {
        const int index_socket = connect_to_server(index);
        if (index_socket < 0) {
            logger.error("Failed to connect to index server");
            return false;
        }
        const RemovePeerDTO dto(peer);
        const string request = to_string(REMOVE_PEER) + " " + dto.serialize();
        stream->write(index_socket, request);
        close(index_socket);
        logger.info("Peer removal request sent to index server");
        return true;
    }

    pair<bool, SearchResult> search_file(const string &filename) override {
        const auto index_socket = connect_to_server(index);
        if (index_socket < 0) {
            logger.error("Failed to connect to index server");
            return make_pair(false, SearchResult());
        }
        const FileSearchDTO dto(peer, filename);
        const string request = to_string(FILE_SEARCH) + " " + dto.serialize();
        stream->write(index_socket, request);
        logger.info("File search request sent to index server");
        const string response = stream->read(index_socket);
        close(index_socket);
        logger.info("File search response received from index server");
        return deserialize_response<SearchResult>(response);
    }

    pair<bool, FileInfo> request_file(const FileDescriptor& descriptor) override {
        const auto index_socket = connect_to_server(index);
        if (index_socket < 0) {
            logger.error("Failed to connect to index server");
            return make_pair(false, FileInfo());
        }
        const FileRequestDTO dto(descriptor);
        const string request = to_string(FILE_REQUEST) + " " + dto.serialize();
        stream->write(index_socket, request);
        logger.info("File request sent to index server");
        const string response = stream->read(index_socket);
        close(index_socket);
        logger.info("File request response received from index server");
        return deserialize_response<FileInfo>(response);
    }

    bool download_file(const FileInfo& info, const string& filename) override {
        const string directory = shared_directory + "/" + to_string(peer.port);
        const string output_file = directory + "/" + filename;
        const size_t numPeers = info.getNumberOfPeersWithFile();
        const size_t totalSize = info.getSize();
        const size_t chunkSize = totalSize / numPeers;
        size_t startByte = 0, mod = totalSize % numPeers;

        if (!filesystem::exists(directory))
            filesystem::create_directory(directory);

        {
            ofstream f(output_file, ios::binary | ios::out);
            f.seekp(totalSize - 1);
            f.write("", 1);
        }

        vector<thread> threads;
        for (const auto& pfi : info.getFileLocation()) {
            const PeerDescriptor currentPeer = pfi.peer;
            const size_t currentChunkSize = chunkSize + (mod > 0 ? 1 : 0);
            const size_t offset = startByte;
            if (mod > 0) mod--;
            startByte += currentChunkSize;
            threads.emplace_back([=, &info, this] {
                const DownloadFileChunkDTO dto(info.getFileDescriptor(), offset, currentChunkSize);
                download_file_chunk(dto, currentPeer, output_file, offset);
            });
        }

        for (auto& t : threads) if (t.joinable()) t.join();
        return true;
    }

};