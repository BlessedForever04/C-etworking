// Socket helpers and shared packet/model declarations.
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "network_manager.h"
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

void *receiveAndBroadcastIncomingData(void *arg){
    int socketFD = *(int*)arg;
    struct response response;
    
    while(1){
        int byteReceived = recv(socketFD, &response, sizeof(struct response), 0);
        if(byteReceived > 0){
            if(strcmp(response.message, "bye\n") == 0){
                printf("%s left the server\n", response.sender); 

                struct response serverNotice;
                snprintf(serverNotice.sender, 7, "%s", "Server");
                snprintf(serverNotice.message, sizeof(response.sender), "%s", response.sender);
                strcat(serverNotice.message, " left the chat.\n");

                for(int i = 0; i < clientList.size; i++){
                    if(clientList.clients[i].clientFD != socketFD){
                        send(clientList.clients->clientFD, &serverNotice, sizeof(serverNotice), 0);
                    }
                }
                removeClientFromClientList(socketFD);
                break;
            }
            else{                
                response.message[byteReceived] = '\0';
                
                for(int i = 0; i < clientList.size; i++){
                    if(clientList.clients[i].clientFD == socketFD) continue;
                    send(clientList.clients[i].clientFD, &response, sizeof(struct response), 0);
                }
            }
        }
    }
    close(socketFD);
    return NULL;
}

void *receiveAndPrintDataFromServer(void *arg){
    int serverSocketFD = *(int*)arg;
    struct response response;
    while(1){
        int byteReceived = recv(serverSocketFD, &response, sizeof(struct response), 0);
        if(byteReceived > 0){
            response.message[byteReceived] = '\0';
            printf("%s: ", response.sender);
            printf("%s", response.message);
        }
    }
    return NULL;
}

void receivingAndBroadcastIncomingDataOnSaperateThread(int clientFD){
    pthread_t clientThread;
    pthread_create(&clientThread, NULL, receiveAndBroadcastIncomingData, &clientFD);
}

void startAcceptingIncomingConnection(int serverSocketFD){
    while(1){
        struct acceptedConnection *acceptedClient = acceptIncomingConnection(serverSocketFD);
        char name[50];
        recv(acceptedClient->FD, name, 50, 0);
        addClientToClientList(name, acceptedClient->FD);
        receivingAndBroadcastIncomingDataOnSaperateThread(acceptedClient->FD);
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