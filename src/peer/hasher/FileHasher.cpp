#include <FileHasher.h>
#include <logger/Logger.h>

void FileHasher::compute_hashes(const string& filepath) const {

    ifstream file(filepath, ios::binary);
    if (!file.is_open()) {
        logger.error("Cannot open file for hashing: " + filepath);
        return;
    }
    
    constexpr ll base = 257, mod = 9223372036854775783LL;
    constexpr ll FNV_OFFSET_BASIS = 14695981039346656037ULL;
    constexpr ll FNV_PRIME = 1099511628211LL;
    
    hash1_value = 0;
    hash2_value = FNV_OFFSET_BASIS;
    file_size = 0;
    
    char buffer[BUFFER_SIZE];
    while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0) {
        size_t bytes_read = file.gcount();
        file_size += bytes_read;
        
        for (size_t i = 0; i < bytes_read; ++i) {
            unsigned char byte = buffer[i];
            hash1_value = (hash1_value * base + byte) % mod;
            hash2_value ^= byte;
            hash2_value = hash2_value * FNV_PRIME % mod;
        }
    }

}

ll FileHasher::get_first_hash() const {
    return hash1_value;
}

ll FileHasher::get_second_hash() const {
    return hash2_value;
}

size_t FileHasher::get_file_size() const {
    return file_size;
}
