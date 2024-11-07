#include <filesystem>
#include <iostream>
#include <utility>
#include "../../Hash/HashGenerator.h"
#include "../PeerFileDTO.h"

vector<PeerFileDTO<ll>>  fileDirectoryReader(int port, string &ip, string &directory_name) {
    const filesystem::path sandbox{directory_name};
    vector<PeerFileDTO<ll>> res;

    for(auto const & dir_entry : filesystem::directory_iterator(sandbox)) {
        
        pair<ll, ll> hash = doubleHashFile(dir_entry.path());
        string name = dir_entry.path().filename();
        PeerFileDTO<ll> DTO;
        DTO.h1 = hash.first;
        DTO.h2 = hash.second;
        DTO.size = filesystem::file_size(dir_entry);
        DTO.alias = name;
        PeerInfo info;
        info.ip.sin6_family = AF_INET6;
        cout<<"Indexing: "<<name<<endl;
        inet_pton(AF_INET6, ip.c_str(), &info.ip.sin6_addr);
        info.ip.sin6_port = htons(8080);
        info.port = port;
        DTO.info = info;
        res.push_back(DTO);
    }
    return res;
}








