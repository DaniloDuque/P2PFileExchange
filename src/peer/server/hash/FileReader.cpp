#include <filesystem>
#include <iostream>
#include <utility>
#include "HashGenerator.h"
#include "../../dto/FileDTO.cpp"
#include "../../../logger/Logger.h"
using namespace std;

vector<FileDTO> fileDirectoryReader(string &directory_name) {
    const std::filesystem::path sandbox{directory_name};
    vector<FileDTO> res;
    
    if (!std::filesystem::exists(sandbox)) {
        logger.error("Directory does not exist: " + directory_name);
        return res;
    }
    
    if (!std::filesystem::is_directory(sandbox)) {
        logger.error("Path is not a directory: " + directory_name);
        return res;
    }
    
    for(auto const & dir_entry : std::filesystem::directory_iterator(sandbox)) {
        pair<ll, ll> hash = doubleHashFile(dir_entry.path());
        string name = dir_entry.path().filename();
        res.emplace_back(hash.first, hash.second, std::filesystem::file_size(dir_entry), name);
    }
    return res;
}








