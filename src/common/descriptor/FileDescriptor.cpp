#pragma once
#define ll long long
#include <sstream>
using namespace std;

struct FileDescriptor {
    ll hash1, hash2;
    size_t size;

    FileDescriptor() = default;

    FileDescriptor(const ll h1, const ll h2, const size_t sz) : hash1(h1), hash2(h2), size(sz) {
    }

    bool operator<(const FileDescriptor &other) const {
        if (hash1 != other.hash1) return hash1 < other.hash1;
        if (hash2 != other.hash2) return hash2 < other.hash2;
        return size < other.size;
    }

    bool operator==(const FileDescriptor &other) const {
        return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
    }

    string serialize() const {
        return to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size);
    }

    static FileDescriptor deserialize(const string &data) {
        if (data.empty()) {
            throw invalid_argument("Empty FileDescriptor data");
        }

        FileDescriptor fd{};
        istringstream ss(data);
        string token;

        if (!getline(ss, token, ',') || token.empty()) {
            throw invalid_argument("Missing hash1 in FileDescriptor");
        }
        try {
            fd.hash1 = stoll(token);
        } catch (const exception &) {
            throw invalid_argument("Invalid hash1 in FileDescriptor");
        }

        if (!getline(ss, token, ',') || token.empty()) {
            throw invalid_argument("Missing hash2 in FileDescriptor");
        }
        try {
            fd.hash2 = stoll(token);
        } catch (const exception &) {
            throw invalid_argument("Invalid hash2 in FileDescriptor");
        }

        if (!getline(ss, token, ',') || token.empty()) {
            throw invalid_argument("Missing size in FileDescriptor");
        }
        try {
            fd.size = stoull(token);
        } catch (const exception &) {
            throw invalid_argument("Invalid size in FileDescriptor");
        }

        return fd;
    }
};
