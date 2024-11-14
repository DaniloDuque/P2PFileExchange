#ifndef NEW_PEER_DTO_HEADER
#define NEW_PEER_DTO_HEADER

#include "FileDTO.h"

template<typename T>
struct NewPeerDTO {
    string ip;
    int port;
    vector<FileDTO<T>> peerFiles;
    
    string serialize() {
        string ser = ip + ',' + to_string(port) + ',';
        for(auto &p : peerFiles) ser += p.serialize();
        return ser;
    }

    static NewPeerDTO<T> deserialize(string &data) {
        NewPeerDTO<T> peerDTO;
        istringstream ss(data);
        string token;
        getline(ss, peerDTO.ip, ',');
        getline(ss, token, ',');
        peerDTO.port = stoi(token); 
        while (getline(ss, token, ',')) {
            peerDTO.peerFiles.push_back(FileDTO<T>::deserialize(token));
        }
        return peerDTO;
    }

};

#endif
