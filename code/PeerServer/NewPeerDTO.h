#ifndef NEW_PEER_DTO_HEADER
#define NEW_PEER_DTO_HEADER

#include <vector>
#include "File.h"

template<typename T>
struct NewPeerDTO {
    string ip;
    int port;
    vector<File<T>> peerFiles;
    
    string serialize() {
        string ser = ip + ',' + to_string(port);
        for(auto &p : peerFiles) ser += " " + p.serialize();
        return ser;
    }

    static NewPeerDTO<T> deserialize(string &data) {
        NewPeerDTO<T> peerDTO;
        istringstream ss(data);  
        string token;
        getline(ss, peerDTO.ip, ',');
        getline(ss, token, ' ');
        peerDTO.port = stoi(token);  
        while (getline(ss, token, ' ')) {
            if (!token.empty()) {  
                peerDTO.peerFiles.push_back(File<T>::deserialize(token));
            }
        }
        return peerDTO;
    }

};

#endif
