#pragma once
#include <iostream>
#include <thread>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <iomanip>
#include <common/descriptor/SearchResult.cpp>

#define ll long long
#define uchar unsigned char
using namespace std;

inline string toLower(string s) {
    ranges::transform(s, s.begin(), [](const unsigned char c) { return tolower(c); });
    return s;
}

inline vector<string> split(const string &input, const char delimiter) {
    vector<string> result;
    stringstream ss(input);
    string item;
    while (getline(ss, item, delimiter)) result.push_back(item);
    return result;
}

inline void print_file_info_table(const SearchResult &data) {
    cout << left << setw(20) << "File Name" << setw(12) << "Size" << setw(22) << "hash1" << setw(22) << "hash2" << endl;
    cout << "------------------------------------------------------------------------------" << endl;
    for (const auto &[filename, descriptor]: data.values) {
        cout << left << setw(20) << filename << setw(12) << descriptor.size << setw(22) << descriptor.hash1 << setw(22)
                << descriptor.hash2 << endl;
    }
}
