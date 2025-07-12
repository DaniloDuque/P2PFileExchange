#pragma once
#include <util.h>
#include <common/descriptor/FileDescriptor.cpp>

struct DownloadFileChunkDTO {
    const FileDescriptor file;
    const size_t start_byte, chunk_size;

    DownloadFileChunkDTO() = delete;
    DownloadFileChunkDTO(const FileDescriptor& file, const size_t startByte, const size_t chunkSize) : file(file), start_byte(startByte), chunk_size(chunkSize) {}

    string serialize() const {
        return to_string(start_byte) + ',' + to_string(chunk_size) + ',' + file.serialize();
    }

    static DownloadFileChunkDTO deserialize(const string& data) {
        istringstream ss(data);
        string token;

        getline(ss, token, ',');
        const size_t startByte = stoll(token);

        getline(ss, token, ',');
        const size_t chunkSize = stoll(token);

        string fd_serialized;
        getline(ss, token, ','); fd_serialized += token + ',';
        getline(ss, token, ','); fd_serialized += token + ',';
        getline(ss, token, ','); fd_serialized += token;

        return DownloadFileChunkDTO{FileDescriptor::deserialize(fd_serialized), startByte, chunkSize};
    }

};
