#pragma once
#include <hasher/FileHasher.h>
#include <common/descriptor/IndexedFileDescriptor.cpp>
#include <filesystem>

class FileIndexer {
    mutable FileHasher hasher;

    vector<unsigned char> read_file(const filesystem::path &path) const;

public:
    FileIndexer() = default;

    set<IndexedFileDescriptor> index_files(const string &path) const;
};
