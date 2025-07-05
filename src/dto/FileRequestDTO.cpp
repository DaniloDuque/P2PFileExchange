#pragma once
#include "../util.h"

struct FileRequestDTO {

    ll hash1, hash2, size, startByte, chunkSize;

    string serialize() const {
        return to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size) + ',' + 
               to_string(startByte) + ',' + to_string(chunkSize); 
    }

    static FileRequestDTO deserialize(const string &data){
        FileRequestDTO dto;
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        dto.hash1 = static_cast<ll>(stoll(token));  
        getline(ss, token, ',');
        dto.hash2 = static_cast<ll>(stoll(token));
        getline(ss, token, ',');
        dto.size = static_cast<ll>(stoll(token));
        getline(ss, token, ',');
        dto.startByte = static_cast<ll>(stoll(token));
        getline(ss, token, ',');
        dto.chunkSize = static_cast<ll>(stoll(token));
        return dto;
    }

};
