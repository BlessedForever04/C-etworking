#include "../../model.h"
#include <stdio.h>
#include "protocol.h"
#include "../client manager/client_manager.h"

void manageServerProtocol(struct packetHeader header, int socketFD){
    switch (header.type){
    case PACKET_CHAT:
        manageBroadcast(socketFD, header);
        break;

    default:
        break;
    }
}

void manageNotice(void){
}

// Change header.type 
// recv payload and not chatPacket and just broadcast the header with payload (Do not parse payload on server side)
void manageBroadcast(int socketFD, struct packetHeader header){
    char *message = malloc(header.payloadSize);
    ssize_t byteReceived = recv(socketFD, message, header.payloadSize, 0);
    
    if(byteReceived > 0){
        for(int i = 0; i < clientList.size; i++){
            if(clientList.clients[i].clientFD != socketFD){
                send(clientList.clients[i].clientFD, &header, sizeof(header), 0);
                send(clientList.clients[i].clientFD, message, header.payloadSize, 0);
            }
        }
    }

    free(message);
}