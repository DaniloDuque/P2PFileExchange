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

#define ll long long
#define uchar unsigned char
using namespace std;

inline string toLower(string s) {
    ranges::transform(s, s.begin(), [](const unsigned char c) { return tolower(c); });
    return s;
}

inline vector<string> split(const string& input, const char delimiter) {
    vector<string> result;
    stringstream ss(input);
    string item;
    while (getline(ss, item, delimiter)) result.push_back(item);
    return result;
}

inline void printFileInfoTable(const string& data) {
    istringstream ss(data);
    string token;
    cout << left << setw(20) << "File Name" << setw(10) << "Size" << endl;
    cout << "---------------------------------------" << endl;
    while (getline(ss, token, ' ')) {
        string fileName;
        string fileSize;
        istringstream pairStream(token);
        getline(pairStream, fileName, ',');
        getline(pairStream, fileSize, ',');
        cout << left << setw(20) << fileName << setw(10) << fileSize << endl;
    }
}
