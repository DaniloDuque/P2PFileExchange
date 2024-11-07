#include "../util.h"
#include "AddPeer.h"
#include "UserListener/UserListener.h"

int main(int argc, char const *argv[]) {
    if (argc < 5) {  
        cerr << "Usage: " << argv[0] << " <mainPort> <indexIp> <indexPort> <directory>" << endl;
        return -1;
    }
    string mainPort = argv[1], indexIp = argv[2], indexPort = argv[3], directory = argv[4];
    thread tadd(&addPeer, ref(indexIp), ref(indexPort), ref(directory));
    PeerServer<ll> server(stoi(mainPort), directory);
    thread tserver(&PeerServer<ll>::run, &server);
    thread tui(&listenUser, ref(indexIp), ref(indexPort));
    tadd.join(); tserver.join(); tui.join(); 
    return 0;
}
