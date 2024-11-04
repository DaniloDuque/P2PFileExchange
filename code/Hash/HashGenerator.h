#include <iostream>
#include <fstream>
#include <string>
#include <utility>

const size_t BUFFER_SIZE = 4096; 


long long polynomialHash(const char* data, size_t length, long long base = 257, long long mod = 9223372036854775783LL) {
    long long hashValue = 0;
    for (size_t i = 0; i < length; ++i) {
        hashValue = (hashValue * base + data[i]) % mod;
    }
    return hashValue;
}


long long fnv1aHash(const char* data, size_t length, long long mod = 9223372036854775783LL) {
    const long long FNV_PRIME = 1099511628211LL;
    const long long FNV_OFFSET_BASIS = 14695981039346656037ULL;
    
    long long hashValue = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < length; ++i) {
        hashValue ^= data[i];
        hashValue = (hashValue * FNV_PRIME) % mod;  
    }
    return hashValue;
}


std::pair<long long int, long long int> doubleHashFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return {-1, -1};
    }

    char buffer[BUFFER_SIZE];
    long long hash1 = 0;
    long long hash2 = 0;
    const long long mod = 9223372036854775783LL;

    
    while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0) {
        size_t bytesRead = file.gcount();

        
        hash1 = (hash1 + polynomialHash(buffer, bytesRead)) % mod;

        
        hash2 = (hash2 + fnv1aHash(buffer, bytesRead)) % mod;
    }

    file.close();
    return {hash1, hash2};
}

