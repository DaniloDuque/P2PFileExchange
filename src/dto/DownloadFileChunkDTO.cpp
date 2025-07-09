#pragma once
#include "util.h"
#include "common/descriptor/FileDescriptor.cpp"

struct DownloadFileChunkDTO {
    FileDescriptor file;
    ll startByte, chunkSize;

    string serialize() const {
        return to_string(startByte) + ',' + to_string(chunkSize) + ',' + file.serialize();
    }

    static DownloadFileChunkDTO deserialize(const string &data) {
        istringstream ss(data);
        string token;

        getline(ss, token, ',');
        const ll startByte = stoll(token);

        getline(ss, token, ',');
        const ll chunkSize = stoll(token);

        string fd_serialized;
        getline(ss, token, ','); fd_serialized += token + ',';
        getline(ss, token, ','); fd_serialized += token + ',';
        getline(ss, token, ','); fd_serialized += token;

        return DownloadFileChunkDTO{FileDescriptor::deserialize(fd_serialized), startByte, chunkSize};
    }

};
