#pragma once
#include "FileHasher.h"

// polynomial hash
ll FileHasher::get_first_hash() const {
    constexpr ll base = 257, mod = 9223372036854775783LL;
    ll hashValue = 0;
    for (auto &i : data) hashValue = (hashValue * base + i) % mod;
    return hashValue;
}

// fnv1a hash
ll FileHasher::get_second_hash() const {
    constexpr ll mod = 9223372036854775783LL;
    constexpr unsigned ll FNV_OFFSET_BASIS = 14695981039346656037ULL;
    ll hashValue = FNV_OFFSET_BASIS;

    for (auto &i : data) {
        constexpr ll FNV_PRIME = 1099511628211LL;
        hashValue ^= i;
        hashValue = hashValue * FNV_PRIME % mod;
    }

    return hashValue;
}

size_t FileHasher::get_file_size() const {
    return data.size();
}

