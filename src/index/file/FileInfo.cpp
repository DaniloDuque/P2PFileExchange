#pragma once
#include <common/fileinfo/CommonFileInfo.h>
#include <common/descriptor/FileLocation.cpp>
#include <common/descriptor/FileDescriptor.cpp>
using namespace std;

class FileInfo final : public CommonFileInfo<FileInfo, FileLocation> {
    void computeLPSArray(const string &pattern, vector<int> &lps) const {
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

    bool KMPIsSubstring(const string &text, const string &pattern) const {
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
        }
        return false;
    }

public:
    FileInfo() = delete;

    FileInfo(const FileDescriptor &descriptor, const FileLocation &location) : CommonFileInfo(descriptor, location) {
    }

    string findMatch(const string &alias) {
        shared_lock lock(infoMutex);
        for (const auto &[_, filename]: file_locations)
            if (KMPIsSubstring(filename, alias))
                return filename;
        return "";
    }

    void removePeer(const PeerDescriptor &peer) {
        unique_lock lock(infoMutex);
        for (auto it = file_locations.begin(); it != file_locations.end();) {
            if (it->peer == peer) it = file_locations.erase(it);
            else ++it;
        }
    }

    bool isEmpty() const {
        shared_lock lock(infoMutex);
        return file_locations.empty();
    }
};
