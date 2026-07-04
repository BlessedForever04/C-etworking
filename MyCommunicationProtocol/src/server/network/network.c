// Socket helpers and shared packet/model declarations.
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "network.h"
#include "../protocol/protocol.h"
#include "../../client/protocol/protocol.h"
#include "../client manager/client_manager.h"

int createTCPIpv4Socket(){
    return socket(AF_INET, SOCK_STREAM, 0); // domain : AF_INET (IP4), type : SOCK_STREAM (TCP), protocol : 0 (Default for TCP)
}

struct sockaddr_in* createSocketAddress(char *ip_address, uint16_t port){
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    inet_pton(AF_INET, ip_address, &address->sin_addr.s_addr);
    return address;
}

void *receiveDataFromClient(void *arg){
    int socketFD = *(int*)arg;
    
    struct packetHeader *header = malloc(sizeof(header));

    while(1){
        int byteReceived = recv(socketFD, header, sizeof(header), 0);
        if(byteReceived > 0){
            manageServerProtocol(*header, socketFD);
        }
    }
    close(socketFD);
    return NULL;
}

void receiveAndManageIncomingDataOnSaperateThread(int clientFD){
    pthread_t clientThread;
    int *clientFDPtr = malloc(sizeof(int));
    *clientFDPtr = clientFD;
    pthread_create(&clientThread, NULL, receiveDataFromClient, clientFDPtr);
    pthread_detach(clientThread);
}

void startAcceptingIncomingConnection(int serverSocketFD){
    while(1){
        struct acceptedConnection *acceptedClient = acceptIncomingConnection(serverSocketFD);
        sendClientListToClient(acceptedClient->FD); // Send list before adding current client
        sendRoomListToClient(acceptedClient->FD);
        addClientToClientList(acceptedClient->FD);
        receiveAndManageIncomingDataOnSaperateThread(acceptedClient->FD);
    }
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