#pragma once
#include <Client.h>
#include <util.h>
#include <peer/server/PeerServer.cpp>
#include <../fileinfo/FileInfo.cpp>
#include <logger/Logger.h>
#include <constants.h>
#include <dto/AddPeerDTO.cpp>
#include <dto/RemovePeerDTO.cpp>
#include <dto/FileSearchDTO.cpp>
#include <dto/FileRequestDTO.cpp>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

class TCPClient final : public Client {
protected:
    static constexpr int MAX_RETRIES = 5;
    static constexpr int RETRY_DELAY_MS = 200;

    int connect_to_server(const PeerDescriptor &peer) override {
        for (int attempt = 1; attempt <= MAX_RETRIES; ++attempt) {
            logger.info(
                "Connecting to " + peer.ip + ":" + to_string(peer.port) + " (attempt " + to_string(attempt) + ")");
            const int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                logger.error("Error creating socket");
                return -1;
            }

            constexpr size_t bufferSize = BUFFER_SIZE;
            setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
            setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));

            sockaddr_in serverAddress{};
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(peer.port);

            if (inet_pton(AF_INET, peer.ip.c_str(), &serverAddress.sin_addr) <= 0) {
                logger.error("Invalid address: " + peer.ip);
                close(sock);
                return -1;
            }

            if (connect(sock, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == 0) {
                logger.info("Connection established to " + peer.ip + ":" + to_string(peer.port));
                return sock;
            }
            logger.warn("Connection failed: " + string(strerror(errno)));
            close(sock);
            if (attempt < MAX_RETRIES) this_thread::sleep_for(chrono::milliseconds(RETRY_DELAY_MS));
        }

        logger.error(
            "Failed to connect to " + peer.ip + ":" + to_string(peer.port) + " after " + to_string(MAX_RETRIES) +
            " attempts");
        return -1;
    }

    template<typename T>
    pair<bool, T> deserialize_response(const string &response) {
        if (response.empty()) return {false, T{}};
        return {true, T::deserialize(response)};
    }

    void download_file_chunk(const DownloadFileChunkDTO &dto, const PeerDescriptor &peer, const string &filename,
                             const size_t offset) {
        const int peerSocket = connect_to_server(peer);
        if (peerSocket < 0) return;

        const string finfo = dto.serialize();
        stream->write(true, peerSocket, finfo);
        const auto [status, file_chunk] = stream->read(peerSocket);

        if (!status) {
            logger.error("Failed to read file chunk: " + file_chunk);
            close(peerSocket);
            return;
        }

        const string decoded_file_chunk = encoder->decode(file_chunk);

        const int fd = open(filename.c_str(), O_WRONLY);
        if (fd < 0) {
            logger.error("Error opening file for writing: " + filename);
            close(peerSocket);
            return;
        }

        if (const ssize_t bytesWritten = pwrite(fd, decoded_file_chunk.data(), decoded_file_chunk.size(), offset);
            bytesWritten < 0) {
            logger.error("Error in pwrite: " + string(strerror(errno)));
        }

        close(fd);
        close(peerSocket);
    }

public:
    TCPClient() = delete;

    TCPClient(const shared_ptr<ByteStream> &stream,
              const shared_ptr<Encoder> &encoder,
              const PeerDescriptor &peer,
              const PeerDescriptor &server,
              const PeerDescriptor &index,
              const string &output_directory)
        : Client(stream, encoder, peer, server, index, output_directory) {
    }

    ~TCPClient() override = default;

    bool add_peer(const set<IndexedFileDescriptor> &shared_files) override {
        const int clientSocket = connect_to_server(index);
        if (clientSocket < 0) return false;

        const AddPeerDTO dto(server, shared_files);

        if (const string package = string(1, ADD_PEER) + " " + dto.serialize(); !stream->write(
            true, clientSocket, package)) {
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
        const RemovePeerDTO dto(server);
        const string request = string(1, REMOVE_PEER) + " " + dto.serialize();
        stream->write(true, index_socket, request);
        close(index_socket);
        logger.info("Peer removal request sent to index server");
        return true;
    }

    pair<bool, SearchResult> search_file(const string &filename) override {
        const auto index_socket = connect_to_server(index);
        if (index_socket < 0) {
            logger.error("Failed to connect to index server");
            return {false, SearchResult{}};
        }
        const FileSearchDTO dto(peer, filename);
        const string request = string(1, FILE_SEARCH) + " " + dto.serialize();
        stream->write(true, index_socket, request);
        logger.info("File search request sent to index server");
        const auto [status, payload] = stream->read(index_socket);
        close(index_socket);
        logger.info("File search response received from index server");
        return deserialize_response<SearchResult>(payload);
    }

    shared_ptr<FileInfo> request_file(const FileDescriptor &descriptor) override {
        const auto index_socket = connect_to_server(index);
        if (index_socket < 0) {
            logger.error("Failed to connect to index server");
            return nullptr;
        }
        const FileRequestDTO dto(descriptor);
        const string request = string(1, FILE_REQUEST) + " " + dto.serialize();
        stream->write(true, index_socket, request);
        logger.info("File request sent to index server");
        const auto [status, payload] = stream->read(index_socket);
        close(index_socket);
        logger.info("File request response received from index server");
        if (payload.empty()) return nullptr;

        istringstream ss(payload);
        string token;
        getline(ss, token, ',');
        ll h1 = stoll(token);
        getline(ss, token, ',');
        ll h2 = stoll(token);
        getline(ss, token, ' ');
        size_t sz = stoll(token);

        auto file_info = make_shared<FileInfo>(h1, h2, sz);
        while (getline(ss, token, ' ')) {
            if (!token.empty())
                file_info->known_as(FileLocation::deserialize(token));
        }
        return file_info;
    }

    bool download_file(const FileInfo &info, const string &filename) override {
        const string directory = output_directory + "/" + to_string(peer.port);
        const string output_file = directory + "/" + filename;
        const size_t numPeers = info.getNumberOfPeersWithFile();
        const size_t totalSize = info.get_file_size();
        const size_t chunkSize = totalSize / numPeers;
        size_t startByte = 0, mod = totalSize % numPeers;

        if (!filesystem::exists(directory))
            filesystem::create_directory(directory); {
            ofstream f(output_file, ios::binary | ios::out);
            f.seekp(totalSize - 1);
            f.write("", 1);
        }

        vector<thread> threads;
        for (const auto &pfi: info.getFileLocation()) {
            const PeerDescriptor currentPeer = pfi.peer;
            const size_t currentChunkSize = chunkSize + (mod > 0 ? 1 : 0);
            const size_t offset = startByte;
            if (mod > 0) mod--;
            startByte += currentChunkSize;
            threads.emplace_back([=, &info, this] {
                const DownloadFileChunkDTO dto(info.get_file_descriptor(), offset, currentChunkSize);
                download_file_chunk(dto, currentPeer, output_file, offset);
            });
        }

        for (auto &t: threads) if (t.joinable()) t.join();
        return true;
    }
};
