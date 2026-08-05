#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "network.h"
#include "../../shared/model.h"
#include "../protocol/protocol.h"
#include "../../shared/recv_all/recv_all.h"

int createTCPIpv4Socket(void){
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in* createSocketAddress(char *ip_address, uint16_t port){
    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    inet_pton(AF_INET, ip_address, &address->sin_addr.s_addr);
    return address;
}

void *receiveDataFromServer(void *arg){ // here the function has to be of *function(*void) type to be used for pthread
    struct pair *socketAndName = (struct pair*)arg;

    int serverSocketFD = socketAndName->socketFD;
    char *myName = socketAndName->name;
    
    struct packetHeader header = {0};

    while(1){
        int byteReceived = recv(serverSocketFD, &header, sizeof(header), 0);
        if(byteReceived > 0){
            manageClientProtocol(header, serverSocketFD, myName);
        }
    }
    return NULL;
}

int getMySocketFD(char *myName){
    for(size_t i = 0; i < userList.size; i++){
        if(strcmp(userList.clients[i].name, myName) == 0) return userList.clients[i].FD;
    }
    return 69;
}