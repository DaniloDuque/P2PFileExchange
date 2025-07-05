#pragma once
#include <vector>
#include "FileDTO.cpp"

struct NewPeerDTO {
    string ip;
    int port{};
    vector<FileDTO> peerFiles;
    
    string serialize() const {
        string ser = ip + ',' + to_string(port);
        for(auto &p : peerFiles) ser += " " + p.serialize();
        return ser;
    }

    static NewPeerDTO deserialize(const string &data) {
        NewPeerDTO peerDTO;
        istringstream ss(data);  
        string token;
        getline(ss, peerDTO.ip, ',');
        getline(ss, token, ' ');
        peerDTO.port = stoi(token);  
        while (getline(ss, token, ' ')) {
            if (!token.empty()) {  
                peerDTO.peerFiles.push_back(FileDTO::deserialize(token));
            }
        }
        return peerDTO;
    }

};
