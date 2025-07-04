#pragma once
#include <string>
#include "../../util.h"

struct FileDTO {
    ll hash1, hash2, size;
    string fileName;

    string serialize() {
        return to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size) + ',' + fileName;
    }

    static FileDTO deserialize(string &data) {
        FileDTO fileDTO;
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        fileDTO.hash1 = static_cast<ll>(stoll(token)); 
        getline(ss, token, ',');
        fileDTO.hash2 = static_cast<ll>(stoll(token));
        getline(ss, token, ',');
        fileDTO.size = static_cast<ll>(stoll(token));  
        getline(ss, fileDTO.fileName); 
        return fileDTO;
    }

};
