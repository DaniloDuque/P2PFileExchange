#pragma once
#include "common/fileinfo/CommonFileInfo.h"
#include "common/descriptor/FileLocation.cpp"
using namespace std;

class FileInfo final : public CommonFileInfo<FileInfo, FileLocation> {
public:
    FileInfo() = delete;
    FileInfo(const FileDescriptor& descriptor, const FileLocation& location) : CommonFileInfo(descriptor, location) {}

    set<FileLocation> getFileInfo() {
        shared_lock lock(infoMutex);
        return file_locations;
    }
    size_t getNumberOfPeersWithFile() const { 
        shared_lock lock(infoMutex);
        return file_locations.size();
    }

};
