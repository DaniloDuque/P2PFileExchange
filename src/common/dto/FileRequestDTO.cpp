#pragma once
#include "common/descriptor/FileDescriptor.cpp"

struct FileRequestDTO {
    const FileDescriptor file;

    FileRequestDTO() = delete;
    FileRequestDTO(const FileDescriptor& file) : file(file) {}

    string serialize() const {
        return file.serialize();
    }

    static FileRequestDTO deserialize(const string& data) {
        return FileRequestDTO(FileDescriptor::deserialize(data));
    }

};