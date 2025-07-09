#pragma once
#include <iostream>
#include <shared_mutex>
using namespace std;

template<typename derived, typename file_location>
class CommonFileInfo {
protected:
    const FileDescriptor descriptor;
    set<file_location> file_locations;
    mutable shared_mutex infoMutex;

public:

    CommonFileInfo(const FileDescriptor& descriptor, const file_location& location): descriptor(descriptor) {
        knownAs(location);
    }
    CommonFileInfo(const ll h1, const ll h2, const ll sz, const file_location& location) : descriptor(h1, h2, sz) {
        knownAs(location);
    };

    void knownAs(const file_location& info) {
        unique_lock lock(infoMutex);
        file_locations.insert(info);
    }

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

    static unique_ptr<derived> deserialize(const string& data) {
        istringstream ss(data);
        string token;
        getline(ss, token, ',');
        ll h1 = stoll(token); 
        getline(ss, token, ',');
        ll h2 = stoll(token); 
        getline(ss, token, ' '); 
        ll sz = stoll(token); 
        auto new_file_locations = make_unique<derived>(h1, h2, sz);
        while (getline(ss, token, ' ')) {
            new_file_locations->knownAs(file_location::deserialize(token));
        }
        return new_file_locations;
    }

    bool operator==(const CommonFileInfo& other) const {
        return descriptor == other.descriptor;
    }

    bool operator<(const CommonFileInfo& other) const {
        return descriptor < other.descriptor;
    }

};