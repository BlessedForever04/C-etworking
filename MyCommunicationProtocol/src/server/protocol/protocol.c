#include <stdint.h>
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
        manageNewRoom(header, sourceClientFD); 
        break;

    case PACKET_JOIN_ROOM:
        manageNewMemberInRoom(header, sourceClientFD);
        break; 

    default:
        break;
    }
}

void manageNewMemberInRoom(struct packetHeader header, int sourceClientFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, sourceClientFD);

    char *memberName = packetReadString(&reader);
    uint8_t *FD = packetReadBytes(&reader, sizeof(int));
    char *groupName = packetReadString(&reader);

    struct group *group = NULL;

    for(size_t i = 0; i < roomList.size; i++){
        if(strcmp(groupName, roomList.group[i].name) == 0){
            group = &roomList.group[i];
            break;
        }
    }

    if(group == NULL){
        free(groupName);
        free(FD);
        free(memberName);
        free(reader.buffer);
        return;
    }

    struct client newMember = {memberName, 0};
    memcpy(&newMember.FD, FD, sizeof(newMember.FD));
    addClientInClientList(&group->members, newMember);

    // Sending the group details to the added member so he can update this group in his gruop list
    sendGroupDetailsToAddedMember(*group, newMember.FD);

    // Sending the new member info to all the group members
    for(size_t i = 0; i < group->members.size; i++){
        send(group->members.clients[i].FD, &header, sizeof(header), 0);
        send(group->members.clients[i].FD, reader.buffer, header.payloadSize, 0);
    }

    free(groupName);
    free(FD);
    free(memberName);
    free(reader.buffer);
}

void sendGroupDetailsToAddedMember(struct group group, int newMemberFD){
    struct packetHeader header;
    header.type = PACKET_GROUP_LIST;
    header.payloadSize = sizeof(uint32_t) +
                         strlen(group.name) +
                         sizeof(uint32_t) +
                         strlen(group.description) +
                         sizeof(uint32_t) +
                         strlen(group.admin.name) +
                         sizeof(int);

    for(size_t i = 0; i < group.members.size; i++){
        header.payloadSize += sizeof(uint32_t) + strlen(group.members.clients[i].name) + sizeof(int); 
    }

    struct packetWriter writer;
    packetWriterInIt(&writer, header.payloadSize);

    // Group name
    packetWriteString(&writer, group.name);
    // Description
    packetWriteString(&writer, group.description);
    // Admin's name
    packetWriteString(&writer, group.admin.name);
    // Admin's FD
    packetWriteBytes(&writer, &group.admin.FD, sizeof(int));

    for(size_t i = 0; i < group.members.size; i++){
        packetWriteString(&writer, group.members.clients[i].name);
        packetWriteBytes(&writer, &group.members.clients[i].FD, sizeof(int));
    }

    send(newMemberFD, &header, sizeof(header), 0);
    send(newMemberFD, writer.buffer, header.payloadSize, 0);
}

void manageNewRoom(struct packetHeader header, int sourceClientFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, sourceClientFD);

    struct group newGroup;
    newGroup.name = packetReadString(&reader); // Group name
    newGroup.description = packetReadString(&reader); // Group description
    newGroup.admin.name = packetReadString(&reader); // Group admin's name
    newGroup.admin.FD = *packetReadBytes(&reader, sizeof(int)); // Group admin's FD

    // Adding group in local list
    addGroupInGroupList(&roomList, newGroup);

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
    printf("User's name is : %s", newClient.name);
    addClientInClientList(&clientList, newClient);
    sendClientListToClient(sourceClientFD);
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
