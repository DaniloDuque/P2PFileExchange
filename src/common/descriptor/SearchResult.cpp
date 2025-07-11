#pragma once
#include <string>
#include <vector>
#include "descriptor/FileDescriptor.cpp"
using namespace std;

struct SearchResult {
    vector<pair<const string&, const FileDescriptor&>> values{};
};