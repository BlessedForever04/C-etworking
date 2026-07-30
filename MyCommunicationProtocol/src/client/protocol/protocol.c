#include "../../model.h"
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include "protocol.h"
#include "../serializer/serializer.h"
#include "../../shared/recv_all.h"

struct clientList userList = {NULL, 0, 0};

void manageClientProtocol(struct packetHeader header, int serverSocketFD){
    switch (header.type){
        case PACKET_CLIENT_LIST:
        receiveClientList(header, serverSocketFD);
        break;
        
        case PACKET_ROOM_LIST:
        receiveRoomList(serverSocketFD, header);
        break;
        
        case PACKET_CHAT:
        receiveAndPrintMessage(serverSocketFD, header);
        break;

        case PACKET_USER_LEFT:
        handleLeftUser(header, serverSocketFD);
        break;
        
        default:
        break;
    }
}

void handleLeftUser(struct packetHeader header, int serverSocketFD){
    printf("Someone left the server\n");
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
    printf("Removed client from the local list\n");
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
        struct client *temp = realloc(list->clients, sizeof(struct client) * list->capacity);
        if(temp == NULL){
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        list->clients = temp;
    }

    list->clients[list->size++] = newClient;
}

void receiveAndPrintMessage(int socketFD, struct packetHeader header){
    printf("Someone sent the message\n");
    struct packetReader reader;
    
    packetReaderInIt(&reader, header.payloadSize, socketFD);
    char *sender = packetReadString(&reader);
    char *message = packetReadString(&reader);
    
    printf("%s: %s", sender, message);
}

void receiveClientList(struct packetHeader header, int socketFD){
    // Receiving the users (clientList)
    struct packetReader reader; 
    packetReaderInIt(&reader, header.payloadSize, socketFD);

    struct client user;
    
    while(reader.size > 0){
        user.name = packetReadString(&reader);
        printf("%s\n", user.name);
        uint8_t *clientFDBytes = packetReadBytes(&reader, sizeof(int));
        memcpy(&user.clientFD, clientFDBytes, sizeof(int));
        free(clientFDBytes);
        addClientToClientList(&userList, user);
    }
    free(reader.buffer); 
}

void receiveRoomList(int socketFD, struct packetHeader header){
    (void)socketFD;
    (void)header;
}