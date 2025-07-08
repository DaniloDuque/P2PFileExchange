#pragma once
#include "util.h"

struct FileDTO {
    ll hash1{}, hash2{}, size{};
    string filename;

    FileDTO() = default;
    FileDTO(const ll h1, const ll h2, const ll sz, const string& alias) : hash1(h1), hash2(h2), size(sz), filename(alias) {}

    string serialize() const {
        return to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size) + ',' + filename;
    }

    static FileDTO deserialize(const string &data) {
        FileDTO file;
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        file.hash1 = stoll(token);
        getline(ss, token, ',');
        file.hash2 = stoll(token);
        getline(ss, token, ',');
        file.size = stoll(token);
        getline(ss, file.filename);
        return file;
    }

    bool operator<(const FileDTO& other) const {
        if (hash1 != other.hash1) return hash1 < other.hash1;
        if (hash2 != other.hash2) return hash2 < other.hash2;
        return size < other.size;
    }

    bool operator==(const FileDTO& other) const {
        return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
    }

};