#include "../util.h"
#include "AddPeer.h"
//#include "FileFinder/FileFinder.cpp"

int main(int argc, char const *argv[]) {
    if (argc < 6) {  
        cerr << "Usage: " << argv[0] << " <port> <indexIp> <indexPort> <directory>" << endl;
        return -1;
    }
    string port=argv[1], ip=argv[2], indexIp=argv[3], indexPort=argv[4], directory=argv[5];
    addPeer(port, ip, indexIp, indexPort, directory);
    //thread tadd(&addPeer, ref(port), ref(ip), ref(indexIp), ref(indexPort), ref(directory));
    //PeerServer<ll> server(stoi(port), ip, directory);
    //thread tserver(&PeerServer<ll>::run, &server);
    //thread tui(&listenUser, ref(indexIp), ref(indexPort)); // TODO: make user interface a thread
    //tadd.join(); //tserver.join(); //tui.join(); 
    return 0;
}
