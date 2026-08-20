#include "../../shared/model.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include "protocol.h"
#include "../../shared/serializer/serializer.h"
#include "../../shared/recv_all/recv_all.h"
#include "../../shared/group/group.h"
#include "../../shared/client_manager/client_manager.h"
#include "../shared_list/shared_list.h"

void manageClientProtocol(struct packetHeader header, int serverSocketFD){
    switch (header.type){
        case PACKET_CLIENT_LIST:
        receiveUserList(header, serverSocketFD);
        break;
        
        case PACKET_GROUP_LIST:
        receiveGroupList(header, serverSocketFD);
        break;
        
        case PACKET_CHAT:
        receiveAndPrintMessage(header, serverSocketFD);
        break;

        case PACKET_USER_LEFT:
        handleLeftUser(header, serverSocketFD);
        break;

        case PACKET_USER_JOINED_SERVER:
        addJoinedUserInUserList(header, serverSocketFD);
        break;
        
        case PACKET_CREATE_ROOM:
        manageNewGroup(header, serverSocketFD);
        break;

        case PACKET_JOIN_ROOM:
        manageNewMemberInGroup(header, serverSocketFD); 
        break;

        case PACKET_GROUP_CHAT:
        manageGroupChat(header, serverSocketFD);

        default:
        // Default case handling
        break;
    }
}

void manageGroupChat(struct packetHeader header, int socketFD){
    struct packetReader reader; 
    packetReaderInIt(&reader, header.payloadSize, socketFD); 
    // grp name, sender name, sent msg
    char *groupName = packetReadString(&reader);
    char *sender = packetReadString(&reader);
    char *message = packetReadString(&reader);

    if(currentCommunication != NULL && strcmp(currentCommunication, groupName) == 0) printf("%s: %s", sender, message);

    free(groupName);
    free(sender);
    free(message);
    free(reader.buffer);
}

void manageNewMemberInGroup(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);

    struct client newMember;
    newMember.name = packetReadString(&reader);
    uint8_t *FD = packetReadBytes(&reader, sizeof(int));
    memcpy(&newMember.FD, FD, sizeof(newMember.FD));
    char *groupName = packetReadString(&reader);

    for(size_t i = 0; i < groupList.size; i++){
        if(strcmp(groupName, groupList.group[i].name) == 0){
            addClientInClientList(&groupList.group[i].members, newMember);
            break;
        }
    }

    free(newMember.name);
    free(groupName);
    free(FD);
    free(reader.buffer);
}

void manageNewGroup(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);
    
    struct group newGroup = {0};
    newGroup.name = packetReadString(&reader);
    newGroup.description = packetReadString(&reader);
    newGroup.admin.name = packetReadString(&reader);
    newGroup.admin.FD = *packetReadBytes(&reader, sizeof(int));

    addGroupInGroupList(&groupList, newGroup);

    // freeing memory
    free(newGroup.name);
    free(newGroup.description);
    free(newGroup.admin.name);
    free(reader.buffer);
}

void addJoinedUserInUserList(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);
    struct client newUser;
    uint8_t *fd = packetReadBytes(&reader, sizeof(int));
    memcpy(&newUser.FD, fd, sizeof(newUser.FD));
    newUser.name = packetReadString(&reader);

    addClientInClientList(&userList, newUser);
    free(reader.buffer);
    free(fd);
    free(newUser.name);
}

void handleLeftUser(struct packetHeader header, int serverSocketFD){
    int *leftUser = malloc(sizeof(int));
    
    size_t receivedBytes = recvAll(serverSocketFD, leftUser, header.payloadSize);

    if(receivedBytes <= 0){
        perror("recv");
        exit(EXIT_FAILURE);
    }
    removeUserFromUserList(*leftUser);
    free(leftUser);
}

void removeUserFromUserList(int leftUserFD){
    for(size_t i = 0; i < userList.size; i++){
        if(userList.clients[i].FD == leftUserFD){
            userList.clients[i] = userList.clients[userList.size - 1];
            free(userList.clients[--userList.size].name);
            break;
        }
    }
}

void receiveAndPrintMessage(struct packetHeader header, int socketFD){
    struct packetReader reader;
    
    packetReaderInIt(&reader, header.payloadSize, socketFD);

    // Just have to receive it
    uint8_t *tempFD = packetReadBytes(&reader, sizeof(int)); // Receiving destinationFD
    char *sender = packetReadString(&reader); // Sender's name
    char *message = packetReadString(&reader); // Sender's message
        
    if(currentCommunication != NULL && strcmp(currentCommunication, sender) == 0) printf("%s: %s", sender, message);

    free(tempFD);
    free(sender);
    free(message);
    free(reader.buffer);
}

void receiveUserList(struct packetHeader header, int socketFD){
    // Receiving the users (clientList)
    struct packetReader reader; 
    packetReaderInIt(&reader, header.payloadSize, socketFD);

    struct client user;
    while(reader.size > 0){
        user.name = packetReadString(&reader);
        uint8_t *fd = packetReadBytes(&reader, sizeof(int));
        memcpy(&user.FD, fd, sizeof(user.FD));
        addClientInClientList(&userList, user);
        free(user.name);
        free(fd);
    }
    
    free(reader.buffer); 
}

void printUserList(char *myName){
    printf("List of connected users:\n------------------------\n");
    int index = 1;
    for(size_t i = 0; i < userList.size; i++){
        if(strcmp(myName, userList.clients[i].name) == 0) continue;
        printf("%d: %s\n", index, userList.clients[i].name);
        index++;
    }
    // }
    printf("------------------------\n");
}

void receiveGroupList(struct packetHeader header, int socketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, socketFD);

    struct group newGroup = {0};
    newGroup.name = packetReadString(&reader);
    newGroup.description = packetReadString(&reader);
    newGroup.admin.name = packetReadString(&reader);
    uint8_t *FD = packetReadBytes(&reader, sizeof(int));
    memcpy(&newGroup.admin.FD, FD, sizeof(int));
    
    struct client groupMember;
    while(reader.size > 0){
        groupMember.name = packetReadString(&reader);
        FD = packetReadBytes(&reader, sizeof(int));
        memcpy(&groupMember.FD, FD, sizeof(int));

        addClientInClientList(&newGroup.members, groupMember);
        free(groupMember.name);
        free(FD);
    }

    addGroupInGroupList(&groupList, newGroup);

    free(newGroup.name);
    free(newGroup.description);
    free(newGroup.admin.name);

    for(size_t i = 0; i < newGroup.members.size; i++){
        free(newGroup.members.clients[i].name);
    }
}
