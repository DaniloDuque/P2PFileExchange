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
        info.ip = ip;
        cout<<"Indexing: "<<name<<endl;
        info.port = port;
        DTO.info = info;
        res.push_back(DTO);
    }
    return res;
}








