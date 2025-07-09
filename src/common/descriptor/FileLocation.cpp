#pragma once
#include "util.h"
#include "PeerDescriptor.cpp"

struct FileLocation {
    PeerDescriptor peer;
    string filename;

    FileLocation() = default;
    FileLocation(const PeerDescriptor& peer, const string& filename) : peer(peer), filename(filename) {}

};