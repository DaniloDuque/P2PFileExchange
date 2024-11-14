#ifndef FILE_DTO_HEADER
#define FILE_DTO_HEADER

#include <string>

template<typename T>
struct FileDTO {
    T hash1, hash2, size;
    string fileName;

    string serialize() {
        return to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size) + ',' + fileName;
    }

    static FileDTO<T> deserialize(string &data) {
        FileDTO<T> fileDTO;
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        fileDTO.hash1 = static_cast<T>(stoll(token)); 
        getline(ss, token, ',');
        fileDTO.hash2 = static_cast<T>(stoll(token));
        getline(ss, token, ',');
        fileDTO.size = static_cast<T>(stoll(token));  
        getline(ss, fileDTO.fileName); 
        return fileDTO;
    }

};

#endif
