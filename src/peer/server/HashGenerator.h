#pragma once
#include "util.h"
#include "logger/Logger.h"

inline ll polynomialHash(const char* data, const size_t length, const ll base = 257, const ll mod = 9223372036854775783LL) {
    ll hashValue = 0;
    for (size_t i = 0; i < length; ++i) {
        hashValue = (hashValue * base + data[i]) % mod;
    }
    return hashValue;
}

inline unsigned ll fnv1aHash(const char* data, const size_t length, const ll mod = 9223372036854775783LL) {
    constexpr unsigned ll FNV_OFFSET_BASIS = 14695981039346656037ULL;
    unsigned ll hashValue = FNV_OFFSET_BASIS;

    for (size_t i = 0; i < length; ++i) {
        constexpr ll FNV_PRIME = 1099511628211LL;
        hashValue ^= data[i];
        hashValue = hashValue * FNV_PRIME % mod;
    }
    return hashValue;
}

inline std::pair<ll, ll> doubleHashFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        logger.error("Could not open the file!");
        return {-1, -1};
    }

    char buffer[BUFFER_SIZE];
    unsigned ll hash1 = 0;
    unsigned ll hash2 = 0;

    while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0) {
        constexpr ll mod = 9223372036854775783LL;
        const size_t bytesRead = file.gcount();
        hash1 = (hash1 + polynomialHash(buffer, bytesRead)) % mod;
        hash2 = (hash2 + fnv1aHash(buffer, bytesRead)) % mod;
    }

    file.close();
    return {hash1, hash2};
}
