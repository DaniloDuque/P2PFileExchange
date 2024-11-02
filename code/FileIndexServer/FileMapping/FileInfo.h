#ifndef FILEINFO_H
#define FILEINFO_H

#include <iostream>
#include <vector>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/trie_policy.hpp>
#include "PeerInfo.h"
using namespace __gnu_pbds;
using namespace std;

template <typename Key, typename Value = null_type>
using trie = trie<Key, Value, trie_string_access_traits<>, pat_trie_tag, trie_prefix_search_node_update>;

template<typename T>
class FileInfo {
private:
    T hash1, hash2, size;
    trie<string> aka;
    vector<PeerInfo> addr; 

public:
    FileInfo(T, T, T, string, PeerInfo);
    FileInfo(T, T, T);
    void knownAs(string, PeerInfo);
    bool exists(string);
    bool operator==(const FileInfo&) const;
    bool operator<(const FileInfo&) const;
};

#include "FileInfo.cpp"

#endif 

