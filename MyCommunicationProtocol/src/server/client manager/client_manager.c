#include "client_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../client/serializer/serializer.h"

struct clientList clientList = {NULL, 0, 0};

void removeClientFromClientList(int clientFD){
    for(int i = 0; i < clientList.size; i++){
        if(clientList.clients[i].clientFD == clientFD){
            free(clientList.clients[clientList.size - 1].name);
            clientList.clients[i] = clientList.clients[--clientList.size];
            break;
        }
    }
}

void addClientToClientList(int clientFD){
    struct packetHeader header;
    recv(clientFD, &header, sizeof(header), 0);
    
    char *name = malloc(header.payloadSize + 1);
    if(name == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    ssize_t byteReceived = recv(clientFD, name, header.payloadSize, 0);
    if(byteReceived <= 0){
        free(name);
        return;
    }
    name[byteReceived] = '\0';

    if(clientList.size == clientList.capacity){
        if(clientList.capacity == 0) clientList.capacity = 1;
        clientList.capacity = clientList.capacity * 2;
        struct client *temp = realloc(clientList.clients, sizeof(struct client) * clientList.capacity);
        if(temp == NULL){
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        else{
            clientList.clients = temp;
        }
    }
    clientList.clients[clientList.size].name = malloc(strlen(name) + 1);
    strcpy(clientList.clients[clientList.size].name, name);
    clientList.clients[clientList.size].clientFD = clientFD;
    clientList.size++;
}

void sendClientListToClient(int clientFD){
    uint32_t payloadSize = 0;
    payloadSize += 5 * clientList.size; 
    
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
    printf("Sent romos\n");
}