#include "../../model.h"
#include <stdio.h>
#include "protocol.h"
#include "../serializer/serializer.h"

void receiveAndPrintMessage(int socketFD, struct packetHeader header);
void receiveClientList(struct packetHeader header, int socketFD);
void manageRoomList(int socketFD, struct packetHeader header);

void manageClientProtocol(struct packetHeader header, int socketFD){
    switch (header.type){
    case PACKET_CLIENT_LIST:
        receiveClientList(header, socketFD);
        break;

    case PACKET_ROOM_LIST:
        manageRoomList(socketFD, header);
        break;
    
    case PACKET_CHAT:
        receiveAndPrintMessage(socketFD, header);
        break;
    
    default:
        break;
    }
}

void receiveAndPrintMessage(int socketFD, struct packetHeader header){
    struct packetReader reader;

    packetReaderInIt(&reader, header.payloadSize, socketFD);
    char *sender = packetReadString(&reader);
    char *message = packetReadString(&reader);

    printf("%s: %s", sender, message);
}

void receiveClientList(struct packetHeader header, int socketFD){
    uint8_t *payload = malloc(header.payloadSize);
    recv(socketFD, payload, header.payloadSize, 0);

}

void manageRoomList(int socketFD, struct packetHeader header){

}