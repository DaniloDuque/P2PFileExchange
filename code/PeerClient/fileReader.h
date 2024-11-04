#include <filesystem>
#include <iostream>
using namespace std;

void fileDirectoryReader() {
    const std::filesystem::path sandbox{"testDirectory"};

    for(auto const & dir_entry : std::filesystem::directory_iterator(sandbox)) {
        std::cout << dir_entry.path() << endl;
    }
}
