#include "IndexServer.h"
#include "PeerFileDTO.h"
#include "../util.h"

int main() {
    std::string serializedData = "10,20,100,example_file,2001:db8::1,8080";
    
    PeerFileDTO<int> peerFileDTO = PeerFileDTO<int>::deserialize(serializedData);
    std::cout << "h1: " << peerFileDTO.h1 << ", h2: " << peerFileDTO.h2 
              << ", size: " << peerFileDTO.size << ", alias: " << peerFileDTO.alias 
              << ", IP: " << sockaddr_in6_to_string(peerFileDTO.info.ip) 
              << ", Port: " << peerFileDTO.info.port << std::endl;

    return 0;
}
