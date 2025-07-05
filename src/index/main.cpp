#include "IndexServer.cpp"
#include "util.h"
#include "logger/Logger.h"

int main(const int argc, char const *argv[]) {
    if (argc != 2) {
        logger.error("Wrong number of arguments");
        return 1;
    }
    const int indexPort = stoi(argv[1]);
    IndexServer server(indexPort);
    server.run();
    return 0;
}
