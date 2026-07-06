#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "network.h"
#include "../../model.h"
#include "../protocol/protocol.h"
#include "../../shared/recv_all.h"

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
    int serverSocketFD = *(int*)arg;
    
    struct packetHeader header = {0};

    while(1){
        int byteReceived = recv(serverSocketFD, &header, sizeof(header), 0);
        if(byteReceived > 0){
            manageClientProtocol(header, serverSocketFD);
        }
    }
    return NULL;
}