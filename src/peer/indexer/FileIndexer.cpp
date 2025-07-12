#include <FileIndexer.h>
#include <logger/Logger.h>

vector<unsigned char> FileIndexer::read_file(const filesystem::path &path) const {
    vector<unsigned char> buffer;
    
    if (!filesystem::exists(path)) {
        logger.error("File does not exist: " + path.string());
        return buffer;
    }
    
    ifstream file(path, ios::binary);
    if (!file.is_open()) {
        logger.error("Could not open the file: " + path.filename().string());
        return buffer;
    }

    file.seekg(0, ios::end);
    if (file.fail()) {
        logger.error("Failed to seek to end of file: " + path.string());
        return buffer;
    }
    
    const size_t size = file.tellg();
    if (size == 0) {
        logger.warn("Empty file: " + path.string());
        return buffer;
    }
    
    file.seekg(0, ios::beg);
    if (file.fail()) {
        logger.error("Failed to seek to beginning of file: " + path.string());
        return buffer;
    }

    buffer.resize(size);
    file.read(reinterpret_cast<char *>(buffer.data()), size);
    if (file.fail() && !file.eof()) {
        logger.error("Failed to read file: " + path.string());
        buffer.clear();
    }
    file.close();

    return buffer;
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
            auto file_binary = read_file(dir_entry.path());
            
            if (file_binary.empty()) {
                logger.warn("Skipping empty or unreadable file: " + name);
                continue;
            }
            
            hasher.set_file(file_binary);
            const auto h1 = hasher.get_first_hash();
            const auto h2 = hasher.get_second_hash();
            logger.info("Indexing file: " + name);
            res.emplace(FileDescriptor(h1, h2, hasher.get_file_size()), name);
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        logger.error("Filesystem error while indexing: " + string(ex.what()));
    }
    return res;
}
