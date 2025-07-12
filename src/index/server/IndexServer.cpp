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

    void add_peer(const AddPeerDTO &dto) {
        index.addPeer(dto);
        heartbeat.updatePeer(dto.peer);
    }

    void remove_peer(const RemovePeerDTO &dto) {
        heartbeat.removePeer(dto.peer);
    }

    SearchResult find_matches(const string &alias) const {
        return index.find(alias);
    }

    shared_ptr<FileInfo> search_file(const FileDescriptor &descriptor) const {
        return index.find(descriptor);
    }

    void handleAddPeer(const string &request) {
        try {
            const AddPeerDTO new_peer = AddPeerDTO::deserialize(request);
            logger.info("New Peer indexing started - " + new_peer.peer.ip + ":" + to_string(new_peer.peer.port));
            add_peer(new_peer);
        } catch (const exception& ex) {
            logger.error("Failed to deserialize AddPeer request: " + string(ex.what()));
        }
    }

    void handleFileRequest(const string &request, const int client_socket) const {
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
        } catch (const exception& ex) {
            logger.error("Failed to handle file request: " + string(ex.what()));
            stream->write(false, client_socket, "Invalid request");
        }
    }

    void handleRemovePeer(const string &request) {
        try {
            const auto dto = RemovePeerDTO::deserialize(request);
            logger.info("Peer removal started - " + dto.peer.ip + ":" + to_string(dto.peer.port));
            remove_peer(dto);
        } catch (const exception& ex) {
            logger.error("Failed to deserialize RemovePeer request: " + string(ex.what()));
        }
    }

    void handleFileSearch(const string &request, const int client_socket) const {
        try {
            const auto file_search_dto = FileSearchDTO::deserialize(request);
            logger.info("Searching for " + file_search_dto.filename + " in the network");
            const auto matches = find_matches(file_search_dto.filename);
            const SearchResultDTO dto(matches);
            if (!stream->write(true, client_socket, dto.serialize())) {
                logger.error("Failed to send search results");
            }
        } catch (const exception& ex) {
            logger.error("Failed to handle file search: " + string(ex.what()));
            stream->write(false, client_socket, "Search failed");
        }
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
            close(client_socket);
            return;
        }
        
        const string request = toLower(payload);
        if (request.empty()) {
            logger.warn("Empty request received");
            close(client_socket);
            return;
        }
        
        if (request.length() < 2) {
            logger.error("Invalid request format: too short");
            close(client_socket);
            return;
        }
        
        const auto data = request.substr(2, request.size());
        const char command = request[0];
        
        try {
            switch (command) {
                case ADD_PEER:
                    handleAddPeer(data);
                    break;
                case FILE_REQUEST:
                    handleFileRequest(data, client_socket);
                    break;
                case REMOVE_PEER:
                    handleRemovePeer(data);
                    break;
                case FILE_SEARCH:
                    handleFileSearch(data, client_socket);
                    break;
                default:
                    logger.warn("Unknown command: " + to_string(command));
                    break;
            }
        } catch (const exception& ex) {
            logger.error("Error handling client request: " + string(ex.what()));
        }
        
        close(client_socket);
    }

public:
    IndexServer(const shared_ptr<ByteStream> &stream, const int port): TCPServer(stream, port) {
    }

    void run() override {
        heartbeat.setDeadPeerCallback([this](const PeerDescriptor &peer) {
            index.removePeer(peer);
        });
        heartbeat.start();
        TCPServer::run();
    }

    ~IndexServer() override {
        heartbeat.stop();
    }
};
