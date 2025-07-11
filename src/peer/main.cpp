#include "util.h"
#include "logger/Logger.h"
#include <csignal>

string server_port, client_port, local_ip, index_ip, index_port;

void signalHandler(const int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        logger.info("Shutting down gracefully...");
        exit(0);
    }
}

int main(const int argc, char const *argv[]) {
    if (argc < 7) {
        logger.error("Usage: " + string(argv[0]) + " <server port> <client port> <Ip> <index Ip> <indexPort> <directory>");
        return -1;
    }
    
    server_port=argv[1]; client_port=argv[2]; local_ip=argv[3]; index_ip=argv[4]; index_port=argv[5];
    string shared_directory=argv[6];
    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    return 0;
}
