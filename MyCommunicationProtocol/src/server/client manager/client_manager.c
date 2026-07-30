#include "client_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../client/serializer/serializer.h"
#include "../../shared/recv_all.h"

struct clientList clientList = {NULL, 0, 0};

void removeClientFromClientList(int clientFD){
    for(int i = 0; i < clientList.size; i++){
        if(clientList.clients[i].clientFD == clientFD){
            clientList.clients[i].clientFD = clientList.clients[clientList.size - 1].clientFD;
            strcpy(clientList.clients[i].name, clientList.clients[clientList.size - 1].name);
            free(clientList.clients[--clientList.size].name);
            break;
        }
    }
}

struct client getClientName(int clientFD){
    struct packetHeader header;
    if(recvAll(clientFD, &header, sizeof(header)) != (ssize_t)sizeof(header)){
        struct client newClient = {NULL, 0};
        return newClient;
    }
    
    struct client newClient = {NULL, 0};

    newClient.name = malloc(header.payloadSize + 1);
    if(newClient.name == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    ssize_t byteReceived = recvAll(clientFD, newClient.name, header.payloadSize);
    if(byteReceived != (ssize_t)header.payloadSize){
        free(newClient.name);
        return newClient;
    }
    newClient.name[header.payloadSize] = '\0';
    newClient.clientFD = clientFD;
    return newClient;
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
        else{
            list->clients = temp;
        }
    }

    list->clients[list->size++] = newClient;
}

void sendClientListToClient(int clientFD){
    uint32_t payloadSize = 0;
    payloadSize += (sizeof(uint32_t) + sizeof(int)) * clientList.size; 
    
    for(int i = 0; i < clientList.size; i++){
        payloadSize += strlen(clientList.clients[i].name);
    }
    
    // Sending the header for clientList
    struct packetHeader header;
    header.type = PACKET_CLIENT_LIST;
    header.payloadSize = payloadSize;
    send(clientFD, &header, sizeof(header), 0);

    struct packetWriter writer;
    packetWriterInIt(&writer, payloadSize);
    
    for(int i = 0; i < clientList.size; i++){
        packetWriteString(&writer, clientList.clients[i].name);
        packetWriteBytes(&writer, &clientList.clients[i].clientFD, 4);
    }

    send(clientFD, writer.buffer, writer.size, 0);
    free(writer.buffer);
}

void sendRoomListToClient(int clientFD){
    (void)clientFD;
    printf("Sent romos\n");
}