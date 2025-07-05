#pragma once
#include "dto/PeerFileInfoDTO.cpp"
#include "common/fileinfo/CommonFileInfo.h"
using namespace std;

class FileInfo final : public CommonFileInfo<FileInfo> {
    static void computeLPSArray(const string& pattern, vector<int>& lps) {
        const auto m = pattern.size();
        int length = 0;
        lps[0] = 0; 
        size_t i = 1;
        while (i < m) {
            if (pattern[i] == pattern[length]) {
                length++;
                lps[i] = length;
                i++;
            } else {
                if (length != 0) length = lps[length - 1];
                else {
                    lps[i] = 0;
                    i++;
                }
            }
        }
    }

    static bool KMPIsSubstring(const string& text, const string& pattern) {
        const auto n = text.size();
        const auto m = pattern.size();
        if (m == 0) return true; 
        if (n == 0) return false; 
        vector<int> lps(m);
        computeLPSArray(pattern, lps);
        size_t i = 0, j = 0;
        while (i < n) {
            if (pattern[j] == text[i]) i++, j++;
            if (j == m) return true;
            if (i < n && pattern[j] != text[i]) {
                if (j != 0) j = lps[j - 1];
                else i++;
            }
        }return false;
    }

public:
    FileInfo(const ll h1, const ll h2, const ll sz) : CommonFileInfo(h1, h2, sz) {}
    FileInfo(const ll h1, const ll h2, const ll sz, const PeerFileInfoDTO& info) : CommonFileInfo(h1, h2, sz, info) {}
    
    string findMatch(const string alias) override {
        for(auto &pfi : fileInfo) 
            if(KMPIsSubstring(pfi.filename, alias))
                return pfi.filename;
        return "";
    }
    
    void removePeer(const string& ip, int port) {
        for (auto it = fileInfo.begin(); it != fileInfo.end();) {
            if (it->ip == ip && it->port == port) {
                it = fileInfo.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    bool isEmpty() const {
        return fileInfo.empty();
    }

};
