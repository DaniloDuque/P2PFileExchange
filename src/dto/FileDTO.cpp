#pragma once
#include "../util.h"
#include <string>

struct FileDTO {
    ll hash1, hash2, size;
    string fileName;

    FileDTO() = default;
    FileDTO(ll h1, ll h2, ll sz, string alias) : hash1(h1), hash2(h2), size(sz), fileName(alias) {}

    string serialize() {
        return to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size) + ',' + fileName;
    }

    static FileDTO deserialize(string &data) {
        FileDTO file;
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        file.hash1 = static_cast<ll>(stoll(token)); 
        getline(ss, token, ',');
        file.hash2 = static_cast<ll>(stoll(token));
        getline(ss, token, ',');
        file.size = static_cast<ll>(stoll(token));  
        getline(ss, file.fileName); 
        return file;
    }

    bool operator<(const FileDTO& other) const {
        if (hash1 != other.hash1) return hash1 < other.hash1;
        if (hash2 != other.hash2) return hash2 < other.hash2;
        return size < other.size;
    }

    bool operator==(const FileDTO& other) const {
        return (hash1 == other.hash1) && (hash2 == other.hash2) && (size == other.size);
    }

};
