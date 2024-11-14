#ifndef FILE_DTO_HEADER
#define FILE_DTO_HEADER

#include <string>

template<typename T>
struct File {
    T hash1, hash2, size;
    string fileName;

    File(T h1, T h2, T sz, string alias) : hash1(h1), hash2(h2), size(sz), fileName(alias) {}

    string serialize() {
        return to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size) + ',' + fileName;
    }

    static File<T> deserialize(string &data) {
        File<T> file;
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        file.hash1 = static_cast<T>(stoll(token)); 
        getline(ss, token, ',');
        file.hash2 = static_cast<T>(stoll(token));
        getline(ss, token, ',');
        file.size = static_cast<T>(stoll(token));  
        getline(ss, file.fileName); 
        return file;
    }

    bool operator<(const File& other) const {
        if (hash1 != other.hash1) return hash1 < other.hash1;
        if (hash2 != other.hash2) return hash2 < other.hash2;
        return size < other.size;
    }

    bool operator==(const File& other) const {
        return (hash1 == other.hash1) && (hash2 == other.hash2) && (size == other.size);
    }

};

#endif
