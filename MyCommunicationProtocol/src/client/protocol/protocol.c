#include "../../model.h"
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include "protocol.h"
#include "../serializer/serializer.h"
#include "../../shared/recv_all.h"

struct clientList userList = {NULL, 0, 0};

void manageClientProtocol(struct packetHeader header, int serverSocketFD, char *myName){
    switch (header.type){
        case PACKET_CLIENT_LIST:
        receiveUserList(header, serverSocketFD, myName);
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
        addJoinedUserToUserList(header, serverSocketFD);
        
        default:
        break;
    }
}

void addJoinedUserToUserList(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);
    struct client newUser;
    uint8_t *fd = packetReadBytes(&reader, sizeof(int));
    memcpy(&newUser.clientFD, fd, sizeof(newUser.clientFD));
    newUser.name = packetReadString(&reader);

    addClientToClientList(&userList, newUser);
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
    for(int i = 0; i < userList.size; i++){
        if(userList.clients[i].clientFD == leftUserFD){
            strcpy(userList.clients[i].name, userList.clients[userList.size - 1].name);
            userList.clients[i].clientFD = userList.clients[userList.size - 1].clientFD;
            free(userList.clients[--userList.size].name);
            break;
        }
    }
}

void addClientToClientList(struct clientList *list, struct client newClient){
    if(list->size == list->capacity){
        if(list->capacity == 0) list->capacity = 1;
        list->capacity = list->capacity * 2;

        /*
            Here list->clients is not directly used for reallocation bcz of the failure risk, its better practice to check temp first and then allocate if 
            everything is good 
        */

        struct client *temp = realloc(list->clients, sizeof(struct client) * list->capacity);
        if(temp == NULL){
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        list->clients = temp;
    }

    list->clients[list->size].clientFD = newClient.clientFD;
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
    uint8_t *tempFD = packetReadBytes(&reader, sizeof(int));
    tempFD = packetReadBytes(&reader, sizeof(int));
    char *sender = packetReadString(&reader);
    char *message = packetReadString(&reader);
    
    free(tempFD);
    
    if(strcmp(currentCommunication, sender) == 0) printf("%s: %s", sender, message);
}

void receiveUserList(struct packetHeader header, int socketFD, char *myName){
    // Receiving the users (clientList)
    struct packetReader reader; 
    packetReaderInIt(&reader, header.payloadSize, socketFD);

    struct client user;
    while(reader.size > 0){
        user.name = packetReadString(&reader);
        uint8_t *fd = packetReadBytes(&reader, sizeof(int));
        memcpy(&user.clientFD, fd, sizeof(user.clientFD));
        addClientToClientList(&userList, user);
        free(user.name);
        free(fd);
    }
    
    free(reader.buffer); 
    printUserList(myName);
}

void printUserList(char *myName){
    printf("List of connected users:\n------------------------\n");
    int index = 1;
    for(int i = 0; i < userList.size; i++){
        if(strcmp(myName, userList.clients[i].name) == 0) continue;
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