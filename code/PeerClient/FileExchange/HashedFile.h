#ifndef HASHED_FILE_HEADER
#define HASHED_FILE_HEADER

#include "../../util.h"

template<typename T>
struct HashedFile {
    T h1, h2, size;
    string alias;

    HashedFile(T h1, T h2, T size, string alias)
        : h1(h1), h2(h2), size(size), alias(alias) {}

    bool operator<(const HashedFile& other) const {
        if (h1 != other.h1) return h1 < other.h1;
        if (h2 != other.h2) return h2 < other.h2;
        return size < other.size;
    }

    bool operator==(const HashedFile& other) const {
        return (h1 == other.h1) && (h2 == other.h2) && (size == other.size);
    }

};

#endif