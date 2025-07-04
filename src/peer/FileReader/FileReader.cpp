#include <filesystem>
#include <iostream>
#include <utility>
#include "Hashing/HashGenerator.h"
#include "../File.h"

template<typename T>
vector<File<T>> fileDirectoryReader(string &directory_name) {
    const filesystem::path sandbox{directory_name};
    vector<File<T>> res;
    for(auto const & dir_entry : filesystem::directory_iterator(sandbox)) {
        pair<ll, ll> hash = doubleHashFile(dir_entry.path());
        string name = dir_entry.path().filename();
        res.emplace_back(hash.first, hash.second, filesystem::file_size(dir_entry), name);
    }
    return res;
}








