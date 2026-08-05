#include <stdio.h>
#include <string.h>

#include "protocol.h"
#include "../../shared/model.h"
#include "../client_manager/client_manager.h"
#include "../../shared/recv_all/recv_all.h"
#include "../../shared/serializer/serializer.h"
#include "../../shared/group/group.h"
#include "../shared_list/shared_list.h"


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

    case PACKET_CREATE_ROOM:
        manageNewGroup(header, sourceClientFD); 
        break;
        
    default:
        break;
    }
}

void manageNewGroup(struct packetHeader header, int sourceClientFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, sourceClientFD);

    struct group newGroup;
    newGroup.name = packetReadString(&reader); // Group name
    newGroup.description = packetReadString(&reader); // Group description
    newGroup.admin.name = packetReadString(&reader); // Group admin's name
    newGroup.admin.FD = *packetReadBytes(&reader, sizeof(int)); // Group admin's FD

    // Adding group in local list
    addGroupInGroupList(roomList, newGroup);
    // Broadcasting newGroup
    for(size_t i = 0; i < clientList.size; i++){
        if(clientList.clients[i].FD != sourceClientFD){
            send(clientList.clients[i].FD, &header, sizeof(header), 0);
            send(clientList.clients[i].FD, reader.buffer, header.payloadSize, 0);
        }
    }

    // freeing memory
    free(reader.buffer);
    free(newGroup.name);
    free(newGroup.description);
    free(newGroup.admin.name);
}

void manageNewClient(struct packetHeader header, int sourceClientFD){
    struct client newClient = getClientName(header, sourceClientFD);
    if(newClient.name == NULL){
        return;
    }
    introduceNewClient(newClient);
    addClientInClientList(&clientList, newClient);
    sendClientListToClient(sourceClientFD);
    sendRoomListToClient(sourceClientFD);
    free(newClient.name);
}

void managePeerChat(struct packetHeader header, int sourceClientFD){
    struct packetReader reader;
    // Reading the received packet from source client
    packetReaderInIt(&reader, header.payloadSize, sourceClientFD);

    uint8_t *pDestinationFD = packetReadBytes(&reader, sizeof(int));
    int destinationFD;
    memcpy(&destinationFD, pDestinationFD, sizeof(destinationFD));

    send(destinationFD, &header, sizeof(header), 0);
    send(destinationFD, reader.buffer, header.payloadSize, 0);
    free(pDestinationFD);
}

void manageLeftClient(int sourceClientFD){
    removeClientFromClientList(sourceClientFD);

    struct packetHeader header;
    header.type = PACKET_USER_LEFT;
    header.payloadSize = sizeof(int);

    for(size_t i = 0; i < clientList.size; i++){
        send(clientList.clients[i].FD, &header, sizeof(header), 0);
        send(clientList.clients[i].FD, &sourceClientFD, sizeof(int), 0);
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
        for(size_t i = 0; i < clientList.size; i++){
            if(clientList.clients[i].FD != sourceClientFD){
                send(clientList.clients[i].FD, &header, sizeof(header), 0);
                send(clientList.clients[i].FD, message, header.payloadSize, 0);
            }
        }
    }
    free(message);
}