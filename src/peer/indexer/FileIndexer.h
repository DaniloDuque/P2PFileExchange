#pragma once
#include "hasher/FileHasher.h"
#include "common/descriptor/IndexedFileDescriptor.cpp"

class FileIndexer {
    FileHasher hasher;
    FileIndexer() = default;
    vector<unsigned char> read_file(const filesystem::path &path) const;
    vector<IndexedFileDescriptor> index_files(const string& path) const;
};
