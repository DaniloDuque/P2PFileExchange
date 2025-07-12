#include <FileIndexer.h>
#include <logger/Logger.h>

vector<unsigned char> FileIndexer::read_file(const filesystem::path &path) const {
    ifstream file(path, ios::binary);
    vector<unsigned char> buffer;
    if (!file.is_open()) {
        logger.error("Could not open the file: " + path.filename().string());
        return buffer;
    }

    file.seekg(0, ios::end);
    const size_t size = file.tellg();
    file.seekg(0, ios::beg);

    buffer.resize(size);
    file.read(reinterpret_cast<char *>(buffer.data()), size);
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

    for (auto const &dir_entry: std::filesystem::directory_iterator(sandbox)) {
        string name = dir_entry.path().filename();
        auto file_binary = read_file(dir_entry.path());
        hasher.set_file(file_binary);
        const auto h1 = hasher.get_first_hash();
        const auto h2 = hasher.get_second_hash();
        logger.info("Indexing file: " + name);
        res.emplace(FileDescriptor(h1, h2, hasher.get_file_size()), name);
    }
    return res;
}
