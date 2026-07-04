#include "../../model.h"
#include <stdio.h>
#include "protocol.h"

void manageClientProtocol(struct packetHeader header, int socketFD){
    switch (header.type){
    case PACKET_CLIENT_LIST:
        manageClientList(socketFD, header);
        break;
    case PACKET_ROOM_LIST:
        manageRoomList(socketFD, header);
        break;
    
    default:
        break;
    }
}

void manageClientList(int socketFD, struct packetHeader header){
    uint8_t *payload = malloc(header.payloadSize);
}

void manageRoomList(int socketFD, struct packetHeader header){

}