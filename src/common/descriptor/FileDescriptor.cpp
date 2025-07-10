#pragma once
#include "util.h"

struct FileDescriptor {
    ll hash1, hash2;
    size_t size;

    FileDescriptor() = default;
    FileDescriptor(const ll h1, const ll h2, const size_t sz) : hash1(h1), hash2(h2), size(sz) {}

    bool operator<(const FileDescriptor& other) const {
        if (hash1 != other.hash1) return hash1 < other.hash1;
        if (hash2 != other.hash2) return hash2 < other.hash2;
        return size < other.size;
    }

    bool operator==(const FileDescriptor& other) const {
        return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
    }

    string serialize() const {
        return to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size);
    }

    static FileDescriptor deserialize(const string &data) {
        FileDescriptor fd{};
        istringstream ss(data);
        string token;
        getline(ss, token, ','); fd.hash1 = stoll(token);
        getline(ss, token, ','); fd.hash2 = stoll(token);
        getline(ss, token, ','); fd.size = stoull(token);
        return fd;
    }
    
};