#include "../../model.h"
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include "protocol.h"
#include "../serializer/serializer.h"
#include "../../shared/recv_all.h"

static struct clientList userList = {NULL, 0, 0};

void manageClientProtocol(struct packetHeader header, int socketFD){
    switch (header.type){
        case PACKET_CLIENT_LIST:
        receiveClientList(header, socketFD);
        break;
        
        case PACKET_ROOM_LIST:
        receiveRoomList(socketFD, header);
        break;
        
        case PACKET_CHAT:
        receiveAndPrintMessage(socketFD, header);
        break;

        case PACKET_USER_LEFT:
        handleLeftUser(socketFD);
        break;
        
        default:
        break;
    }
}

void handleLeftUser(int socketFD){
    printf("Someone left the server\n");
    int *leftUser = malloc(sizeof(int));
    int receivedBytes = recv(socketFD, leftUser, sizeof(int), 0);
    if(receivedBytes <= 0){
        perror("recv");
        exit(EXIT_FAILURE);
    }
    removeClientFromClientList(*leftUser);
}

void removeClientFromClientList(int clientFD){
    printf("Removed client from the local list\n");
    for(int i = 0; i < userList.size; i++){
        if(userList.clients[i].clientFD == clientFD){
            free(userList.clients[userList.size - 1].name);
            userList.clients[i] = userList.clients[--userList.size];
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
    uint8_t *payload = malloc(header.payloadSize);
    struct packetReader reader;
    if(payload == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    ssize_t byteReceived = recv(socketFD, payload, header.payloadSize, 0);
    if(byteReceived <= 0){
        free(payload);
        return;
    }

    reader.buffer = payload;
    reader.offset = payload;
    reader.size = header.payloadSize;
    
    struct client user;
    
    while(reader.size > 0){
        user.name = packetReadString(&reader);
        printf("%s\n", user.name);
        uint8_t *clientFDBytes = packetReadBytes(&reader, sizeof(int));
        memcpy(&user.clientFD, clientFDBytes, sizeof(int));
        free(clientFDBytes);
        addClientToClientList(&userList, user);
    }

    free(payload);
}

void receiveRoomList(int socketFD, struct packetHeader header){
    (void)socketFD;
    (void)header;
}