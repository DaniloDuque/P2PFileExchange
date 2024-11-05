#include "PeerServer.h"





int main() {

    vector<PeerFileDTO<long long>> list;

    std::string directory = "FileReader/testDirectory";

    list = fileDirectoryReader(directory);
    std::string package = "1 ";
    for(int i = 0; i < list.size(); i++) {
        package += list[i].serialize();
        if(i != list.size() - 1) {
            package += ' ';
        }

    }
    std::cout << package << endl;



    return 0;
}
