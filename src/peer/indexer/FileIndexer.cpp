#include <FileIndexer.h>
#include <logger/Logger.h>

bool FileIndexer::validate_file(const filesystem::path &path) const {
    if (!filesystem::exists(path)) {
        logger.error("File does not exist: " + path.string());
        return false;
    }
    
    if (!filesystem::is_regular_file(path)) {
        logger.warn("Not a regular file: " + path.string());
        return false;
    }
    
    ifstream file(path, ios::binary);
    if (!file.is_open()) {
        logger.error("Could not open the file: " + path.filename().string());
        return false;
    }
    
    return true;
}

set<IndexedFileDescriptor> FileIndexer::index_files(const string &path) const {
    const std::filesystem::path sandbox{path};
    set<IndexedFileDescriptor> res;

    if (!std::filesystem::exists(sandbox)) {
        logger.error("Directory does not exist: " + path);
        return res;
    }

    if (!std::filesystem::is_directory(sandbox)) {
        logger.error("Path is not a directory: " + path);
        return res;
    }

    try {
        for (auto const &dir_entry: std::filesystem::directory_iterator(sandbox)) {
            if (!dir_entry.is_regular_file()) continue;
            
            string name = dir_entry.path().filename();
            
            if (!validate_file(dir_entry.path())) {
                logger.warn("Skipping invalid file: " + name);
                continue;
            }
            
            hasher.compute_hashes(dir_entry.path().string());
            const auto h1 = hasher.get_first_hash();
            const auto h2 = hasher.get_second_hash();
            const auto size = hasher.get_file_size();
            
            if (size == 0) {
                logger.warn("Skipping empty file: " + name);
                continue;
            }
            
            logger.info("Indexing file: " + name);
            res.emplace(FileDescriptor(h1, h2, size), name);
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        logger.error("Filesystem error while indexing: " + string(ex.what()));
    }
    return res;
}
