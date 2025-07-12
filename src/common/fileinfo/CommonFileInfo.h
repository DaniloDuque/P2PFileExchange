#pragma once
#include <iostream>
#include <shared_mutex>
#include "common/descriptor/FileDescriptor.cpp"
using namespace std;

template<typename derived, typename Location>
class CommonFileInfo {
protected:
    const FileDescriptor descriptor{};
    set<Location> file_locations{};
    mutable shared_mutex infoMutex;

public:
    void known_as(const Location& info) {
        unique_lock lock(infoMutex);
        file_locations.insert(info);
    }

    CommonFileInfo(const FileDescriptor& descriptor, const Location& location): descriptor(descriptor) {
        known_as(location);
    }

    CommonFileInfo(const FileDescriptor& descriptor): descriptor(descriptor) {}

    CommonFileInfo() = default;
    CommonFileInfo(const CommonFileInfo&) = delete;
    CommonFileInfo& operator=(const CommonFileInfo&) = delete;
    CommonFileInfo(CommonFileInfo&&) = default;
    CommonFileInfo& operator=(CommonFileInfo&&) = default;

    ll get_first_hash() const { return descriptor.hash1; }
    ll get_second_hash() const { return descriptor.hash2; }
    ll get_file_size() const { return descriptor.size; }

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
        derived new_file_locations(h1, h2, sz);
        while (getline(ss, token, ' ')) {
            if (!token.empty())
                new_file_locations.known_as(Location::deserialize(token));
        }
        return std::move(new_file_locations);
    }

    bool operator==(const CommonFileInfo& other) const {
        return descriptor == other.descriptor;
    }

    bool operator<(const CommonFileInfo& other) const {
        return descriptor < other.descriptor;
    }

    FileDescriptor get_file_descriptor() const {
        return descriptor;
    }

    shared_ptr<FileDescriptor> get_file_descriptor_ptr() const {
        return make_shared<FileDescriptor>(descriptor);
    }

};