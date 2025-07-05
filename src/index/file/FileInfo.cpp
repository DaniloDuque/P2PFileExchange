#pragma once
#include "PeerFileInfo.cpp"
using namespace std;

class FileInfo {
private:
    const ll hash1, hash2, size;
    set<PeerFileInfo> fileInfo; 

    void computeLPSArray(const string& pattern, vector<int>& lps) {
        int m = pattern.size();
        int length = 0;
        lps[0] = 0; 
        int i = 1;
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

    bool KMPIsSubstring(const string& text, const string& pattern) {
        int n = text.size(), m = pattern.size();
        if (m == 0) return true; 
        if (n == 0) return false; 
        vector<int> lps(m);
        computeLPSArray(pattern, lps);
        int i = 0, j = 0; 
        while (i < n) {
            if (pattern[j] == text[i]) i++, j++;
            if (j == m) return true;
            else if (i < n && pattern[j] != text[i]) {
                if (j != 0) j = lps[j - 1];
                else i++;
            }
        }return false;
    }

public:
    FileInfo(ll h1, ll h2, ll sz, PeerFileInfo info) : hash1(h1), hash2(h2), size(sz) {
        knownAs(info);
    };

    FileInfo(ll h1, ll h2, ll sz) : hash1(h1), hash2(h2), size(sz) {};

    ll getSize() const { return size; }
    ll getHash1() const { return hash1; }
    ll getHash2() const { return hash2; }

    void knownAs(PeerFileInfo info) {
        fileInfo.insert(info);
    }

    string findMatch(string alias) {
        for(auto &pfi : fileInfo) 
            if(KMPIsSubstring(pfi.filename, alias))
                return pfi.filename;
        return "";
    }

    string serialize() const {
        string rsp = to_string(hash1) + ',' + to_string(hash2) + ',' + to_string(size); 
        for(auto &pf : fileInfo) rsp += " " + pf.serialize();
        return rsp;
    }       

    static FileInfo deserialize(const string& data) {
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        int h1 = (stoll(token)); 
        getline(ss, token, ',');
        int h2 = (stoll(token)); 
        getline(ss, token, ' '); 
        int sz = (stoll(token)); 
        FileInfo newFileInfo(h1, h2, sz);
        while (getline(ss, token, ' ')) {
            PeerFileInfo peerFileInfo = PeerFileInfo::deserialize(token);
            newFileInfo.knownAs(peerFileInfo);
        }
        return newFileInfo;
    }

    bool operator==(const FileInfo& other) const {
        return hash1 == other.hash1 && hash2 == other.hash2 && size == other.size;
    }

    bool operator<(const FileInfo& other) const {
        if (hash1 != other.hash1) return hash1 < other.hash1;
        if (hash2 != other.hash2) return hash2 < other.hash2;
        return size < other.size;
    }

};
