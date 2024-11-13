#include "../../util.h"

template<typename T>
struct File {
    string filename;
    T hash1, hash2, size;
    File(string name, T h1, T h2, T sz) : filename(name), hash1(h1), hash2(h2), size(sz) {} 
};

