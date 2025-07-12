#pragma once
#include <string>
#include <vector>
#include "descriptor/FileDescriptor.cpp"
using namespace std;

struct SearchResult {
    vector<pair<string, FileDescriptor>> values{};
    
    static SearchResult deserialize(const string& data) {
        istringstream ss(data);
        size_t count;
        ss >> count;
        SearchResult result;
        result.values.reserve(count);

        string entry;
        while (ss >> entry) {
            size_t pos1 = entry.find(',');
            size_t pos2 = entry.find(',', pos1 + 1);

            if (size_t pos3 = entry.find(',', pos2 + 1); pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) continue;

            string filename = entry.substr(0, pos1);
            string descriptor_str = entry.substr(pos1 + 1);
            FileDescriptor fd = FileDescriptor::deserialize(descriptor_str);
            result.values.emplace_back(filename, fd);
        }

        return result;
    }

};