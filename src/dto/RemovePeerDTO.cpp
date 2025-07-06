#pragma once
#include <sstream>
#include <string>
using namespace std;

struct RemovePeerDTO {
    string ip;
    int port{};

    string serialize() const { return ip + ',' + to_string(port); }

    static RemovePeerDTO deserialize(const string &data) {
        RemovePeerDTO peerDTO;
        istringstream ss(data);  
        string token;
        getline(ss, peerDTO.ip, ',');
        getline(ss, token, ' ');
        peerDTO.port = stoi(token);
        return peerDTO;
    }

};
