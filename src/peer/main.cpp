#include "util.h"
#include "server/AddPeer.cpp"
#include "server/RemovePeer.cpp"
#include "logger/Logger.h"
#include <csignal>

string g_port, g_ip, g_indexIp, g_indexPort;

void signalHandler(const int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        logger.info("Shutting down gracefully...");
        removePeer(g_ip, g_indexIp, g_indexPort, g_port);
        exit(0);
    }
}

int main(const int argc, char const *argv[]) {
    if (argc < 6) {  
        logger.error("Usage: " + string(argv[0]) + " <port> <indexIp> <indexPort> <directory>");
        return -1;
    }
    g_port=argv[1]; g_ip=argv[2]; g_indexIp=argv[3]; g_indexPort=argv[4];
    string directory=argv[5];
    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    thread tadd(&addPeer, ref(g_port), ref(g_ip), ref(g_indexIp), ref(g_indexPort), ref(directory));
    PeerServer server(stoi(g_port), directory);
    thread tserver(&PeerServer::run, &server);
    tadd.join(); tserver.join(); 
    return 0;
}
