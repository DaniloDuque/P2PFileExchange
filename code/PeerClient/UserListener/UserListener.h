#ifndef USER_LISTENER_HEADER
#define USER_LISTENER_HEADER

#include "../../util.h"

int connectToIndex(string &indexIp, string &indexPort){

    int indexSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (indexSocket < 0) {
        cerr << "Error creating socket" << endl;
        return -1;
    }

    struct hostent *server = gethostbyname(indexIp.c_str());
    if (server == NULL) {
        cerr << "Error: could not resolve server address" << endl;
        close(indexSocket);
        return -1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(stoi(indexPort));  
    memcpy(&serverAddress.sin_addr.s_addr, server->h_addr_list[0], server->h_length);  

    if (connect(indexSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error connecting to the server" << endl;
        close(indexSocket);
        return -1;
    }

    return indexSocket;

}

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
        int indexSocket = connectToIndex(ip, port);
        string fileInfo = getFileInfo(indexSocket, input);
        cout<<fileInfo<<endl;
    }while(input!="EXIT");
}

#endif
