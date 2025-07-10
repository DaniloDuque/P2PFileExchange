#pragma once
#include <string>
#include <vector>

struct SearchResult {
    vector<pair<const string&, const FileDescriptor&>> values{};
};