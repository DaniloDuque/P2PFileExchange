#ifndef USER_LISTENER_HEADER
#define USER_LISTENER_HEADER

#include "../../util.h"
#include "../PeerServer.h"

string getFileInfo(int indexSocket, string &filename){
    string package = "2 " + filename;
    if (send(indexSocket, package.c_str(), package.size(), 0) < 0) {
        cerr << "Error sending the package" << endl;
        close(indexSocket);
        return "";
    }
    puts("File info requested!");
    char buffer[BUFFER_SIZE] = {};
    read(indexSocket, buffer, BUFFER_SIZE);
    string rqst = buffer;
    package = "0";
    send(indexSocket, package.c_str(), package.size(), 0);
    close(indexSocket);
    return rqst;
}

void listenUser(string &ip, string &port){
    string input;
    do{
        cout<<"Insert the name of the file to search in index server: ";
        cin>>input;
        cout<<endl;
        int indexSocket = PeerServer<ll>::connectToIndex(ip, port);
        string fileInfo = getFileInfo(indexSocket, input);
        cout<<fileInfo<<endl;
    }while(input!="EXIT");
}

#endif
