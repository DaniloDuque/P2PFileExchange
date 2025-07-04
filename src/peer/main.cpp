#include "../util.h"
#include "server/AddPeer.cpp"
#include "../logger/Logger.h"

int main(int argc, char const *argv[]) {
    if (argc < 6) {  
        logger.error("Usage: " + string(argv[0]) + " <port> <indexIp> <indexPort> <directory>");
        return -1;
    }
    string port=argv[1], ip=argv[2], indexIp=argv[3], indexPort=argv[4], directory=argv[5];
    thread tadd(&addPeer, ref(port), ref(ip), ref(indexIp), ref(indexPort), ref(directory));
    PeerServer server(stoi(port), directory);
    thread tserver(&PeerServer::run, &server);
    tadd.join(); tserver.join(); 
    return 0;
}
