#include <filesystem>
#include <iostream>
#include <utility>
#include "HashGenerator.h"
#include "../../common/descriptor/IndexedFileDescriptor.cpp"
#include "logger/Logger.h"
using namespace std;

vector<IndexedFileDescriptor> fileDirectoryReader(const string &directory_name) {
    const std::filesystem::path sandbox{directory_name};
    vector<IndexedFileDescriptor> res;
    
    if (!std::filesystem::exists(sandbox)) {
        logger.error("Directory does not exist: " + directory_name);
        return res;
    }
    
    if (!std::filesystem::is_directory(sandbox)) {
        logger.error("Path is not a directory: " + directory_name);
        return res;
    }
    
    for(auto const & dir_entry : std::filesystem::directory_iterator(sandbox)) {
        auto [h1, h2] = doubleHashFile(dir_entry.path());
        string name = dir_entry.path().filename();
        FileDescriptor desc(h1, h2, static_cast<ll>(std::filesystem::file_size(dir_entry)));
        res.emplace_back(desc, name);
    }
    return res;
}