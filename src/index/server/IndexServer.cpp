#pragma once
#include <util.h>
#include <common/server/TCPServer.cpp>
#include <index/fileindex/FileIndex.cpp>
#include <index/file/FileInfo.cpp>
#include <dto/AddPeerDTO.cpp>
#include <dto/RemovePeerDTO.cpp>
#include <heartbeat/HeartbeatManager.cpp>
#include <dto/FileSearchDTO.cpp>
#include <dto/FileRequestDTO.cpp>
#include <dto/SearchResultDTO.cpp>
#include <descriptor/SearchResult.cpp>
#include <constants.h>

class IndexServer final : public TCPServer {
    FileIndex index;
    HeartbeatManager heartbeat;
    static constexpr int METADATA_SIZE_LIMIT = 1024 * 1024;

    void add_peer(const AddPeerDTO &dto) {
        index.add_peer(dto);
        heartbeat.update_peer(dto.peer);
    }

    void remove_peer(const RemovePeerDTO &dto) {
        heartbeat.remove_peer(dto.peer);
    }

    SearchResult find_matches(const string &alias) const {
        return index.find(alias);
    }

    shared_ptr<FileInfo> search_file(const FileDescriptor &descriptor) const {
        return index.find(descriptor);
    }

    void handle_add_peer(const string &request) {
        try {
            const AddPeerDTO new_peer = AddPeerDTO::deserialize(request);
            logger.info("New Peer indexing started - " + new_peer.peer.ip + ":" + to_string(new_peer.peer.port));
            add_peer(new_peer);
        } catch (const exception &ex) {
            logger.error("Failed to deserialize AddPeer request: " + string(ex.what()));
        }
    }

    void handle_file_request(const string &request, const int client_socket) const {
        try {
            const auto [descriptor] = FileRequestDTO::deserialize(request);
            logger.info(
                "Peer requested file - " + to_string(descriptor.hash1) + " " + to_string(descriptor.hash2) + " " +
                to_string(descriptor.size));
            const auto requested_file_info = search_file(descriptor);
            string rsp;
            if (requested_file_info != nullptr) rsp = requested_file_info->serialize();
            if (!stream->write(true, client_socket, rsp)) {
                logger.error("Failed to send file request response");
            }
        } catch (const exception &ex) {
            logger.error("Failed to handle file request: " + string(ex.what()));
            stream->write(false, client_socket, "Invalid request");
        }
    }

    void handle_remove_peer(const string &request) {
        try {
            const auto dto = RemovePeerDTO::deserialize(request);
            logger.info("Peer removal started - " + dto.peer.ip + ":" + to_string(dto.peer.port));
            remove_peer(dto);
        } catch (const exception &ex) {
            logger.error("Failed to deserialize RemovePeer request: " + string(ex.what()));
        }
    }

    void handle_file_search(const string &request, const int client_socket) const {
        try {
            const auto file_search_dto = FileSearchDTO::deserialize(request);
            logger.info("Searching for " + file_search_dto.filename + " in the network");
            const auto matches = find_matches(file_search_dto.filename);
            const SearchResultDTO dto(matches);
            if (!stream->write(true, client_socket, dto.serialize())) {
                logger.error("Failed to send search results");
            }
        } catch (const exception &ex) {
            logger.error("Failed to handle file search: " + string(ex.what()));
            stream->write(false, client_socket, "Search failed");
        }
    }

    bool validate_request(const string &request, char &command, string &data) const {
        if (request.empty()) {
            logger.warn("Empty request received");
            return false;
        }

        if (request.length() < 3) {
            logger.error("Request too short: " + to_string(request.length()));
            return false;
        }

        command = request[0];
        if (request[1] != ' ') {
            logger.error("Invalid request format: missing space separator");
            return false;
        }

        if (command != ADD_PEER && command != FILE_REQUEST &&
            command != REMOVE_PEER && command != FILE_SEARCH) {
            logger.error("Invalid command: " + to_string(command));
            return false;
        }

        data = request.substr(2);
        if (data.empty()) {
            logger.error("Empty data payload");
            return false;
        }

        if (data.length() > METADATA_SIZE_LIMIT) {
            logger.error("Data payload too large: " + to_string(data.length()));
            return false;
        }

        return true;
    }

    void handle_client(const int client_socket) override {
        if (client_socket < 0) {
            logger.error("Invalid client socket");
            return;
        }

        logger.info("Request received!");
        const auto [status, payload] = stream->read(client_socket);

        if (!status) {
            logger.error("Failed to read from client: " + payload);
            stream->write(false, client_socket, "Read error");
            close(client_socket);
            return;
        }

        char command;
        string data;
        if (!validate_request(payload, command, data)) {
            stream->write(false, client_socket, "Invalid request format");
            close(client_socket);
            return;
        }

        try {
            switch (command) {
                case ADD_PEER:
                    handle_add_peer(data);
                    break;
                case FILE_REQUEST:
                    handle_file_request(data, client_socket);
                    break;
                case REMOVE_PEER:
                    handle_remove_peer(data);
                    break;
                case FILE_SEARCH:
                    handle_file_search(data, client_socket);
                    break;
                default:
                    logger.error("Unhandled command: " + to_string(static_cast<int>(command)));
                    stream->write(false, client_socket, "Unknown command");
                    break;
            }
        } catch (const exception &ex) {
            logger.error("Error handling client request: " + string(ex.what()));
            stream->write(false, client_socket, "Server error");
        }

        close(client_socket);
    }

public:
    IndexServer(const shared_ptr<ByteStream> &stream, const int port): TCPServer(stream, port) {
    }

    void run() override {
        heartbeat.set_dead_peer_callback([this](const PeerDescriptor &peer) {
            index.remove_peer(peer);
        });
        heartbeat.start();
        TCPServer::run();
    }

    ~IndexServer() override {
        heartbeat.stop();
    }
};
