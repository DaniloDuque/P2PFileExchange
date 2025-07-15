#pragma once
#include <hasher/FileHasher.h>
#include <common/descriptor/IndexedFileDescriptor.cpp>
#include <filesystem>

class FileIndexer {
    mutable FileHasher hasher;

    bool validate_file(const filesystem::path &path) const;

public:
    FileIndexer() = default;

    set<IndexedFileDescriptor> index_files(const string &path) const;
};
