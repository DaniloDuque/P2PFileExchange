#include <filesystem>
#include <iostream>
#include <utility>
#include "../Hash/HashGenerator.h"

std::vector<std::pair<long long, long long>>  fileDirectoryReader(std::string & directory_name) {
    const std::filesystem::path sandbox{directory_name};
    std::vector<std::pair<long long, long long>> res;

    for(auto const & dir_entry : std::filesystem::directory_iterator(sandbox)) {
        
        std::pair<long long int, long long int> hash = doubleHashFile(dir_entry.path());
        std::cout << hash.first << ' ' << hash.second <<' ' << std::filesystem::file_size(dir_entry) << std::endl;
        res.push_back(hash);
    }
    return res;
}








