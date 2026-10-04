#include "client_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../serializer/serializer.h"
#include "../recv_all/recv_all.h"

void removeClientFromClientList(struct clientList *clientList, int targetFD){
    for(size_t i = 0; i < clientList->size; i++){
        if(clientList->clients[i].FD == targetFD){
            clientList->clients[i] = clientList->clients[clientList->size - 1];
            free(clientList->clients[--clientList->size].name);
            break;
        }
    }
}

struct client getClientName(struct packetHeader header, int clientFD){
    struct client newClient = {NULL, 0};

    newClient.name = malloc(header.payloadSize + 1);
    if(newClient.name == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    ssize_t byteReceived = recvAll(clientFD, newClient.name, header.payloadSize);
    if(byteReceived != (ssize_t)header.payloadSize){
        free(newClient.name);
        newClient.name = NULL;
        return newClient;
    }
    newClient.name[header.payloadSize] = '\0';
    newClient.FD = clientFD;
    return newClient;
}

void addClientInClientList(struct clientList *clientList, struct client newClient){
    if(clientList->size == clientList->capacity){
        if(clientList->capacity == 0) clientList->capacity = 1;
        clientList->capacity = clientList->capacity * 2;
        struct client *temp = realloc(clientList->clients, sizeof(struct client) * clientList->capacity);
        if(temp == NULL){
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        else{
            clientList->clients = temp;
        }
    }

    clientList->clients[clientList->size].FD = newClient.FD;
    clientList->clients[clientList->size].name = malloc(strlen(newClient.name) + 1);
    if(clientList->clients[clientList->size].name == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(clientList->clients[clientList->size].name, newClient.name);
    clientList->size++;
}

void sendClientListToClient(struct clientList clientList, int clientFD){
    uint32_t payloadSize = 0;
    //            (clientNameLen    +    socketFD) * number of connected clients
    payloadSize = (sizeof(uint32_t) + sizeof(int)) * clientList.size; 
    
    for(size_t i = 0; i < clientList.size; i++){
        payloadSize += strlen(clientList.clients[i].name);
    }
    
    // Sending the header for clientList
    struct packetHeader header;
    header.type = PACKET_CLIENT_LIST;
    header.payloadSize = payloadSize;
    send(clientFD, &header, sizeof(header), 0);

    struct packetWriter writer;
    packetWriterInIt(&writer, payloadSize);
    
    for(size_t i = 0; i < clientList.size; i++){
        packetWriteString(&writer, clientList.clients[i].name);
        packetWriteBytes(&writer, &clientList.clients[i].FD, sizeof(int));
    }

    send(clientFD, writer.buffer, writer.size, 0);
    free(writer.buffer);
}

void introduceNewClient(struct clientList clientList, struct client newClient){
    struct packetHeader header;
    header.type = PACKET_USER_JOINED_SERVER;
    // SocketFD + string length prefix + name string
    header.payloadSize = sizeof(int) + sizeof(uint32_t) + strlen(newClient.name);

    struct packetWriter writer;
    packetWriterInIt(&writer, header.payloadSize);                   // Creating buffer
    packetWriteBytes(&writer, &newClient.FD, sizeof(int));     // Storing socket FD
    packetWriteString(&writer, newClient.name);                      // Storing name

    for(size_t i = 0; i < clientList.size; i++){
        // sending header
        send(clientList.clients[i].FD, &header, sizeof(header), 0);
        // Sending buffer
        send(clientList.clients[i].FD, writer.buffer, header.payloadSize, 0);
    }
    free(writer.buffer);
}
