#include <filesystem>
#include <iostream>
#include <utility>
#include "../Hash/HashGenerator.h"

void fileDirectoryReader() {
    const std::filesystem::path sandbox{"testDirectory"};

    for(auto const & dir_entry : std::filesystem::directory_iterator(sandbox)) {
        
        std::pair<long long int, long long int> hash = doubleHashFile(dir_entry.path());
        std::cout << hash.first << ' ' << hash.second << ' ' << std::filesystem::file_size(dir_entry) << std::endl;

    }
}


