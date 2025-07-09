#pragma once
#include "util.h"
#include "FileDescriptor.cpp"

struct IndexedFileDescriptor {
    FileDescriptor file{};
    string filename;

    IndexedFileDescriptor() = default;
    IndexedFileDescriptor(const FileDescriptor& fd) : file(fd) {}
    IndexedFileDescriptor(const ll h1, const ll h2, const ll sz, const string& alias) : file(h1, h2, sz), filename(alias) {}
    IndexedFileDescriptor(const FileDescriptor& fd, const string& alias) : file(fd), filename(alias) {}

    string serialize() const {
        return filename + ',' + file.serialize();
    }

    static IndexedFileDescriptor deserialize(const string &data) {
        const size_t pos = data.find(',');
        const string filename = data.substr(0, pos);
        const string descriptorPart = data.substr(pos + 1);
        return IndexedFileDescriptor(FileDescriptor::deserialize(descriptorPart), filename);
    }

    bool operator<(const IndexedFileDescriptor& other) const {
        return file < other.file;
    }

    bool operator==(const IndexedFileDescriptor& other) const {
        return file == other.file;
    }

};