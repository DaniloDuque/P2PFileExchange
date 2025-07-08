#include "util.h"
#include "server/AddPeer.cpp"
#include "server/RemovePeer.cpp"
#include "logger/Logger.h"
#include <csignal>

string server_port, client_port, local_ip, index_ip, index_port;

void signalHandler(const int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        logger.info("Shutting down gracefully...");
        removePeer(local_ip, index_ip, index_port, server_port);
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
    
    thread tadd(&addPeer, ref(server_port), ref(local_ip), ref(index_ip), ref(index_port), ref(shared_directory));
    PeerServer server(stoi(server_port), shared_directory);
    thread tserver(&PeerServer::run, &server);
    tadd.join(); tserver.join(); 
    return 0;
}
