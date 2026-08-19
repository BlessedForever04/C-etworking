#include "../../shared/model.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include "protocol.h"
#include "../../shared/serializer/serializer.h"
#include "../../shared/recv_all/recv_all.h"
#include "../../shared/group/group.h"
#include "../shared_list/shared_list.h"

void manageClientProtocol(struct packetHeader header, int serverSocketFD){
    switch (header.type){
        case PACKET_CLIENT_LIST:
        receiveUserList(header, serverSocketFD);
        break;
        
        case PACKET_ROOM_LIST:
        receiveRoomList(header, serverSocketFD);
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

        default:
        // Default case handling
        break;
    }
}

void manageNewMemberInGroup(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);

    struct client newMember;
    uint8_t *FD = packetReadBytes(&reader, sizeof(int));
    newMember.FD = *FD;
    newMember.name = packetReadString(&reader); 
    char *groupName = packetReadString(&reader);

    for(size_t i = 0; i < groupList.size; i++){
        if(strcmp(groupName, groupList.group[i].name) == 0){
            addUserInUserList(&groupList.group[i].members, newMember);
            break;
        }
    }

    free(newMember.name);
    free(groupName);
    free(FD);
}

void manageNewGroup(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);
    
    struct group newGroup;
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

    addUserInUserList(&userList, newUser);
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
    removeClientFromUserList(*leftUser);
    free(leftUser);
}

void removeClientFromUserList(int leftUserFD){
    for(size_t i = 0; i < userList.size; i++){
        if(userList.clients[i].FD == leftUserFD){
            userList.clients[i] = userList.clients[userList.size - 1];
            free(userList.clients[--userList.size].name);
            break;
        }
    }
}

void addUserInUserList(struct clientList *list, struct client newClient){
    if(list->size == list->capacity){
        if(list->capacity == 0) list->capacity = 1;
        list->capacity = list->capacity * 2;

        struct client *temp = realloc(list->clients, sizeof(struct client) * list->capacity);
        if(temp == NULL){
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        list->clients = temp;
    }

    list->clients[list->size].FD = newClient.FD;
    list->clients[list->size].name = malloc(strlen(newClient.name) + 1);
    if(list->clients[list->size].name == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(list->clients[list->size].name, newClient.name);
    list->size++;
}

void receiveAndPrintMessage(struct packetHeader header, int socketFD){
    struct packetReader reader;
    
    packetReaderInIt(&reader, header.payloadSize, socketFD);

    // Just have to receive it
    uint8_t *tempFD = packetReadBytes(&reader, sizeof(int)); // Receiving destinationFD
    tempFD = packetReadBytes(&reader, sizeof(int)); // Receiving sourceFD
    char *sender = packetReadString(&reader); // Sender's name
    char *message = packetReadString(&reader); // Sender's message
    
    free(tempFD);
    
    if(currentCommunication != NULL && strcmp(currentCommunication, sender) == 0) printf("%s: %s", sender, message);
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
        addUserInUserList(&userList, user);
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
        printf("%s comparing with %s\n", myName, userList.clients[i].name);
        printf("%d: %s\n", index, userList.clients[i].name);
        index++;
    }
    // }
    printf("------------------------\n");
}

void receiveRoomList(struct packetHeader header, int socketFD){
    (void)socketFD;
    (void)header;
}
