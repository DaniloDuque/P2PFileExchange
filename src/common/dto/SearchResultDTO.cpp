#pragma once
#include "constants.h"
#include "descriptor/SearchResult.cpp"

struct SearchResultDTO {
    const SearchResult result;

    SearchResultDTO() = delete;
    SearchResultDTO(const SearchResult& result) : result(result) {}

    string serialize() const {
        string rsp = to_string(result.values.size());
        for (const auto& [filename, descriptor] : result.values)
            rsp += " " + filename + ',' + descriptor.serialize();
        return rsp;
    }

    static SearchResultDTO deserialize(const string& data) {
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

        return SearchResultDTO{result};
    }

};