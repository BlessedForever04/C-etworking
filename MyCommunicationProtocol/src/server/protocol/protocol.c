#include <stdio.h>
#include <string.h>

#include "protocol.h"
#include "../../model.h"
#include "../client_manager/client_manager.h"
#include "../../shared/recv_all.h"
#include "../../client/serializer/serializer.h"

void manageServerProtocol(struct packetHeader header, int sourceClientFD){
    switch (header.type){
    case PACKET_CHAT:
        managePeerChat(header, sourceClientFD);
        break;

    case PACKET_USER_LEFT:
        manageLeftClient(sourceClientFD);
        break;

    case PACKET_INTRO:
        manageNewClient(header, sourceClientFD);
        break;
        
    default:
        break;
    }
}

void manageNewClient(struct packetHeader header, int sourceClientFD){
    struct client newClient = getClientName(header, sourceClientFD);
    if(newClient.name == NULL){
        return;
    }
    introduceNewClient(newClient);
    addClientToClientList(&clientList, newClient);
    sendClientListToClient(sourceClientFD);
    sendRoomListToClient(sourceClientFD);
    free(newClient.name);
}

void managePeerChat(struct packetHeader header, int sourceClientFD){
    printf("Message by %d\n", sourceClientFD);
    struct packetReader reader;
    packetReaderInIt(&reader, sizeof(int), sourceClientFD);

    uint8_t *temp = packetReadBytes(&reader, sizeof(int));
    int destinationFD;
    memcpy(&destinationFD, temp, sizeof(destinationFD));

    free(temp);

    uint8_t *buffer = malloc(header.payloadSize);
    size_t receivedBytes = recvAll(sourceClientFD, buffer, header.payloadSize);

    if(receivedBytes <= 0){
        perror("recv");
        exit(EXIT_FAILURE);
    }

    send(destinationFD, buffer, header.payloadSize, 0);
}

void manageLeftClient(int sourceClientFD){
    removeClientFromClientList(sourceClientFD);

    struct packetHeader header;
    header.type = PACKET_USER_LEFT;
    header.payloadSize = sizeof(int);

    int FD = sourceClientFD;

    for(int i = 0; i < clientList.size; i++){
        send(clientList.clients[i].clientFD, &header, sizeof(header), 0);
        send(clientList.clients[i].clientFD, &FD, sizeof(int), 0);
    }
}

void manageNotice(void){

}

// Change header.type 
// recv payload and not chatPacket and just broadcast the header with payload (Do not parse payload on server side)
void manageBroadcast(int sourceClientFD, struct packetHeader header){
    char *message = malloc(header.payloadSize);
    ssize_t byteReceived = recvAll(sourceClientFD, message, header.payloadSize);
    
    if(byteReceived == (ssize_t)header.payloadSize){
        for(int i = 0; i < clientList.size; i++){
            if(clientList.clients[i].clientFD != sourceClientFD){
                send(clientList.clients[i].clientFD, &header, sizeof(header), 0);
                send(clientList.clients[i].clientFD, message, header.payloadSize, 0);
            }
        }
    }
    free(message);
}