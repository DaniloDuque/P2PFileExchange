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
        const AddPeerDTO new_peer = AddPeerDTO::deserialize(request);
        logger.info("New Peer indexing started - " + new_peer.peer.ip + ":" + to_string(new_peer.peer.port));
        add_peer(new_peer);
    }

    void handleFileRequest(const string &request, const int client_socket) const {
        const auto [descriptor] = FileRequestDTO::deserialize(request);
        logger.info(
            "Peer requested file - " + to_string(descriptor.hash1) + " " + to_string(descriptor.hash2) + " " +
            to_string(descriptor.size));
        const auto requested_file_info = search_file(descriptor);
        string rsp;
        if (requested_file_info != nullptr) rsp = requested_file_info->serialize();
        stream->write(true, client_socket, rsp);
    }

    void handleRemovePeer(const string &request) {
        const auto dto = RemovePeerDTO::deserialize(request);
        logger.info("Peer removal started - " + dto.peer.ip + ":" + to_string(dto.peer.port));
        remove_peer(dto);
    }

    void handleFileSearch(const string &request, const int client_socket) const {
        const auto file_search_dto = FileSearchDTO::deserialize(request);
        logger.info("Searching for " + file_search_dto.filename + " in the network");
        const auto matches = find_matches(file_search_dto.filename);
        const SearchResultDTO dto(matches);
        stream->write(true, client_socket, dto.serialize());
    }

    void handle_client(const int client_socket) override {
        logger.info("Request received!");
        const auto [status, payload] = stream->read(client_socket);
        const string request = toLower(payload);
        if (request.empty()) return;
        const auto data = request.substr(2, request.size());
        if (request[0] == ADD_PEER) handleAddPeer(data);
        if (request[0] == FILE_REQUEST) handleFileRequest(data, client_socket);
        if (request[0] == REMOVE_PEER) handleRemovePeer(data);
        if (request[0] == FILE_SEARCH) handleFileSearch(data, client_socket);
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
