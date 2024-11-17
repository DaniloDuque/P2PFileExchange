#include "../util.h"
#include "AddPeer.h"

int main(int argc, char const *argv[]) {
    if (argc < 6) {  
        cerr << "Usage: " << argv[0] << " <port> <indexIp> <indexPort> <directory>" << endl;
        return -1;
    }
    string port=argv[1], ip=argv[2], indexIp=argv[3], indexPort=argv[4], directory=argv[5];
    thread tadd(&addPeer<ll>, ref(port), ref(ip), ref(indexIp), ref(indexPort), ref(directory));
    PeerServer<ll> server(stoi(port), directory);
    thread tserver(&PeerServer<ll>::run, &server);
    tadd.join(); tserver.join(); 
    return 0;
}
