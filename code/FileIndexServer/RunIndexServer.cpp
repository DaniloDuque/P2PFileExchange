#include "IndexServer.h"
#include "../util.h"

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        std::cerr << "Use: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    int port = std::stoi(argv[1]);
    IndexServer<ll> server(port);
    server.run();
    return 0;
}
