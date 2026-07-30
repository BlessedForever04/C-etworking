#include "../../model.h"
#include <stdio.h>
#include "protocol.h"
#include "../client manager/client_manager.h"
#include "../../shared/recv_all.h"

void manageServerProtocol(struct packetHeader header, int socketFD){
    switch (header.type){
    case PACKET_CHAT:
        manageBroadcast(socketFD, header);
        break;

    case PACKET_USER_LEFT:
        manageLeftClient(socketFD);
        break;

    default:
        break;
    }
}

void manageLeftClient(int socketFD){
    removeClientFromClientList(socketFD);

    struct packetHeader header;
    header.type = PACKET_USER_LEFT;
    header.payloadSize = sizeof(int);

    int FD = socketFD;

    for(int i = 0; i < clientList.size; i++){
        send(clientList.clients[i].clientFD, &header, sizeof(header), 0);
        send(clientList.clients[i].clientFD, &FD, sizeof(int), 0);
    }
}

void manageNotice(void){

}

// Change header.type 
// recv payload and not chatPacket and just broadcast the header with payload (Do not parse payload on server side)
void manageBroadcast(int socketFD, struct packetHeader header){
    char *message = malloc(header.payloadSize);
    ssize_t byteReceived = recvAll(socketFD, message, header.payloadSize);
    
    if(byteReceived == (ssize_t)header.payloadSize){
        for(int i = 0; i < clientList.size; i++){
            if(clientList.clients[i].clientFD != socketFD){
                send(clientList.clients[i].clientFD, &header, sizeof(header), 0);
                send(clientList.clients[i].clientFD, message, header.payloadSize, 0);
            }
        }
    }
    free(message);
}