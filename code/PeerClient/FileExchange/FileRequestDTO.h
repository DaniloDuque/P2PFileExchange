#ifndef FILE_REQUEST_DTO_HEADER
#define FILE_REQUEST_DTO_HEADER

#include "../../util.h"

template<typename T>
struct FileRequestDTO {

    T h1, h2, size;
    T startByte, chunkSize;

    string serialize() const {
        return to_string(h1) + ',' + to_string(h2) + ',' + to_string(size) + ',' + 
               to_string(startByte) + ',' + to_string(chunkSize); 
    }

    static FileRequestDTO<T> deserialize(const string &data){
        FileRequestDTO<T> dto;
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        dto.h1 = static_cast<T>(stoll(token));  
        getline(ss, token, ',');
        dto.h2 = static_cast<T>(stoll(token));
        getline(ss, token, ',');
        dto.size = static_cast<T>(stoll(token));
        getline(ss, token, ',');
        dto.startByte = static_cast<T>(stoll(token));
        getline(ss, token, ',');
        dto.chunkSize = static_cast<T>(stoll(token));
        return dto;
    }

};

#endif