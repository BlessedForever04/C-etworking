#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

struct clientList{    
    struct client *clients;
    int size;
    int capacity;
};

struct client{
    char name[50];
    int clientFD;
};

struct acceptedConnection{
    int FD;
    struct sockaddr_in socketAddress;
    bool acceptedSuccessfuly;
    int error;
};

struct sockaddr_in* createSocketAddress(char *ip_address, uint16_t port){
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    inet_pton(AF_INET, ip_address, &address->sin_addr.s_addr);
    return address;
}

struct acceptedConnection* acceptIncomingConnection(int serverSocketFD){
    struct acceptedConnection *acceptedClient = malloc(sizeof(struct acceptedConnection));
    
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(struct sockaddr_in);
    
    acceptedClient->FD = accept(serverSocketFD, (struct sockaddr*) &clientAddress, &clientAddressSize);
    acceptedClient->socketAddress = clientAddress;
    acceptedClient->acceptedSuccessfuly = (acceptedClient->FD>0);
    
    if(!acceptedClient->acceptedSuccessfuly){
        acceptedClient->error = acceptedClient->FD;
    }
    
    return acceptedClient;
}