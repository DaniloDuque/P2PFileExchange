#include "PeerServer.h"





int main() {

    vector<PeerFileDTO<long long>> list;

    std::string directory = "FileReader/testDirectory";

    list = fileDirectoryReader(directory);
    for(auto i : list) {

        std::cout << i.serialize() << std::endl;

    }



    return 0;
}
