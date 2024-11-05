#ifndef PEER_FILE_DTO_HEADER 
#define PEER_FILE_DTO_HEADER

#include "../util.h"
#include "PeerInfo.h"

template <typename T>
struct PeerFileDTO {
    T h1, h2, size;
    std::string alias;
    PeerInfo info;
    std::string serialize() const {
        return to_string(h1) + ',' + to_string(h2) + ',' + to_string(size) + ',' + alias + ',' +  info.serialize();
    }

    static PeerFileDTO deserialize(const string& data) {
        PeerFileDTO<T> peerFileDTO;
        istringstream ss(data);
        string h1_str, h2_str, size_str;
        getline(ss, h1_str, ',');
        getline(ss, h2_str, ',');
        getline(ss, size_str, ',');
        getline(ss, peerFileDTO.alias, ',');
        peerFileDTO.h1 = static_cast<T>(stoi(h1_str));
        peerFileDTO.h2 = static_cast<T>(stoi(h2_str));
        peerFileDTO.size = static_cast<T>(stoi(size_str));
        string peerInfo_str;
        getline(ss, peerInfo_str); 
        peerFileDTO.info = PeerInfo::deserialize(peerInfo_str);
        return peerFileDTO;
    }
};









#endif 
