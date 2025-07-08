#pragma once
#include "util.h"

struct SearchFileDTO {
    string filename, ip;
    int port;

    string serialize() const {
        return filename + ',' + ip + ',' + to_string(port);
    }

    static SearchFileDTO deserialize(const string &data) {
        SearchFileDTO dto{};
        istringstream ss(data);
        string token;
        getline(ss, dto.filename, ',');
        getline(ss, dto.ip, ',');
        getline(ss, token, ',');
        dto.port = stoi(token);
        return dto;
    }

};