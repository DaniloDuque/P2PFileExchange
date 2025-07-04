#include "../../../util.h"

ll polynomialHash(const char* data, size_t length, ll base = 257, ll mod = 9223372036854775783LL) {
    ll hashValue = 0;
    for (size_t i = 0; i < length; ++i) {
        hashValue = (hashValue * base + data[i]) % mod;
    }
    return hashValue;
}


ll fnv1aHash(const char* data, size_t length, ll mod = 9223372036854775783LL) {
    const ll FNV_PRIME = 1099511628211LL;
    const ll FNV_OFFSET_BASIS = 14695981039346656037ULL;
    
    ll hashValue = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < length; ++i) {
        hashValue ^= data[i];
        hashValue = (hashValue * FNV_PRIME) % mod;  
    }
    return hashValue;
}


std::pair<ll, ll> doubleHashFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!" << std::endl;
        return {-1, -1};
    }

    char buffer[BUFFER_SIZE];
    ll hash1 = 0;
    ll hash2 = 0;
    const ll mod = 9223372036854775783LL;
    
    while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0) {
        size_t bytesRead = file.gcount();
        hash1 = (hash1 + polynomialHash(buffer, bytesRead)) % mod;
        hash2 = (hash2 + fnv1aHash(buffer, bytesRead)) % mod;
    }

    file.close();
    return {hash1, hash2};
}

