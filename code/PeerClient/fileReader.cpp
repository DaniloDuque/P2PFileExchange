#include <filesystem>
#include <iostream>
#include <utility>
#include "../Hash/HashGenerator.h"
#include "PeerFileDTO.h"

std::vector<PeerFileDTO<long long>>  fileDirectoryReader(std::string & directory_name) {
    const std::filesystem::path sandbox{directory_name};
    std::vector<PeerFileDTO<long long>> res;

    for(auto const & dir_entry : std::filesystem::directory_iterator(sandbox)) {
        
        std::pair<long long int, long long int> hash = doubleHashFile(dir_entry.path());
        std::cout << hash.first << ' ' << hash.second <<' ' << std::filesystem::file_size(dir_entry) << std::endl;
        std::string name = dir_entry.path().filename();
        PeerFileDTO<long long> DTO;
        DTO.h1 = hash.first;
        DTO.h1 = hash.second;
        DTO.size = std::filesystem::file_size(dir_entry);
        DTO.alias = name;
        PeerInfo info;
        info.ip.sin6_family = AF_INET6;
        info.ip.sin6_port = htons(8080);
        info.port = 8080;
        DTO.info = info;
        




        
        
        res.push_back(DTO);
    }
    return res;
}








