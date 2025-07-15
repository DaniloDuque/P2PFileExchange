#pragma once
#include <util.h>
using namespace std;

class FileHasher {
    static constexpr size_t BUFFER_SIZE = 8192;
    mutable ll hash1_value = 0;
    mutable ll hash2_value = 0;
    mutable size_t file_size = 0;

public:
    ~FileHasher() = default;
    FileHasher() = default;

    void compute_hashes(const string& filepath) const;
    ll get_first_hash() const;
    ll get_second_hash() const;
    size_t get_file_size() const;
};
