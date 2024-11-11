#include "IndexServer.h"
#include "../util.h"

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        std::cerr << "Use: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    string indexIp = argv[1];
    int indexPort = stoi(argv[2]);
    IndexServer<ll> server(indexPort, indexIp);
    server.run();
    return 0;
}
