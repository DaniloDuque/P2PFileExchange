#pragma once
#include <util.h>
using namespace std;

class FileHasher {
    vector<unsigned char> data{};

public:
    ~FileHasher() = default;

    FileHasher() = default;

    void set_file(const vector<unsigned char> &file_data) { data = file_data; }
    ll get_first_hash() const;

    ll get_second_hash() const;

    size_t get_file_size() const;
};
