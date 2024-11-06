#include <filesystem>
#include <iostream>
#include <utility>
#include "../../Hash/HashGenerator.h"
#include "../PeerFileDTO.h"

std::vector<PeerFileDTO<long long>>  fileDirectoryReader(std::string & directory_name) {
    const std::filesystem::path sandbox{directory_name};
    std::vector<PeerFileDTO<long long>> res;

    for(auto const & dir_entry : std::filesystem::directory_iterator(sandbox)) {
        
        std::pair<long long int, long long int> hash = doubleHashFile(dir_entry.path());
        std::string name = dir_entry.path().filename();
        PeerFileDTO<long long> DTO;
        DTO.h1 = hash.first;
        DTO.h2 = hash.second;
        DTO.size = std::filesystem::file_size(dir_entry);
        DTO.alias = name;
        PeerInfo info;
        info.ip.sin6_family = AF_INET6;

        std::string ip_str = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
        inet_pton(AF_INET6, ip_str.c_str(), &info.ip.sin6_addr);
        info.ip.sin6_port = htons(8080);
        info.port = 8080;
        DTO.info = info;
        




        
        
        res.push_back(DTO);
    }
    return res;
}








