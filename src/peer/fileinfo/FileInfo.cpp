#pragma once
#include <common/fileinfo/CommonFileInfo.h>
#include <common/descriptor/FileLocation.cpp>
using namespace std;

class FileInfo final : public CommonFileInfo<FileInfo, FileLocation> {
public:
    FileInfo() = default;

    FileInfo(const FileDescriptor &descriptor, const FileLocation &location) : CommonFileInfo(descriptor, location) {
    }

    FileInfo(const ll h1, const ll h2, const size_t sz) : CommonFileInfo(FileDescriptor(h1, h2, sz)) {
    }

    set<FileLocation> getFileLocation() const {
        shared_lock lock(infoMutex);
        return file_locations;
    }

    size_t getNumberOfPeersWithFile() const {
        shared_lock lock(infoMutex);
        return file_locations.size();
    }
};
