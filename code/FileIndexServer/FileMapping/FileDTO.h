#ifndef FILE_DTO_HEADER
#define FILE_DTO_HEADER

template<typename T>
struct FileDTO {
    T hash1, hash2, size;
    string fileName;
};

#endif
