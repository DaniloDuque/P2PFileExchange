#pragma once
#include <iostream>
#include <shared_mutex>
#include "common/descriptor/FileDescriptor.cpp"
using namespace std;

template<typename derived, typename file_location>
class CommonFileInfo {
protected:
    const FileDescriptor descriptor{};
    set<file_location> file_locations{};
    mutable shared_mutex infoMutex;

public:

    void knownAs(const file_location& info) {
        unique_lock lock(infoMutex);
        file_locations.insert(info);
    }

    CommonFileInfo(const FileDescriptor& descriptor, const file_location& location): descriptor(descriptor) {
        knownAs(location);
    }

    CommonFileInfo() = default;
    CommonFileInfo(const CommonFileInfo&) = delete;
    CommonFileInfo& operator=(const CommonFileInfo&) = delete;
    CommonFileInfo(CommonFileInfo&&) = delete;
    CommonFileInfo& operator=(CommonFileInfo&&) = delete;

    ll getHash1() const { return descriptor.hash1; }
    ll getHash2() const { return descriptor.hash2; }
    ll getSize() const { return descriptor.size; }

    virtual ~CommonFileInfo() = default;
    
    string serialize() const {
        shared_lock lock(infoMutex);
        string rsp = to_string(descriptor.hash1) + ',' + to_string(descriptor.hash2) + ',' + to_string(descriptor.size);
        for(auto &pf : file_locations) rsp += " " + pf.serialize();
        return rsp;
    }       

    static derived deserialize(const string& data) {
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        ll h1 = stoll(token); 
        getline(ss, token, ',');
        ll h2 = stoll(token); 
        getline(ss, token, ' '); 
        size_t sz = stoll(token);
        auto new_file_locations = derived(h1, h2, sz);
        while (getline(ss, token, ' ')) {
            new_file_locations.knownAs(file_location::deserialize(token));
        }
        return new_file_locations;
    }

    bool operator==(const CommonFileInfo& other) const {
        return descriptor == other.descriptor;
    }

    bool operator<(const CommonFileInfo& other) const {
        return descriptor < other.descriptor;
    }

    FileDescriptor getFileDescriptor() const {
        return descriptor;
    }

    shared_ptr<FileDescriptor> getFileDescriptorPtr() const {
        return make_shared<FileDescriptor>(descriptor);
    }

};