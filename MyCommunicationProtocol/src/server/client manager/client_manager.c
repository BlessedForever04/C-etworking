#include "client_manager.h"
#include <stdlib.h>
#include <string.h>

struct clientList clientList = {NULL, 0, 0};

void removeClientFromClientList(int clientFD){
    for(int i = 0; i < clientList.size; i++){
        if(clientList.clients[i].clientFD == clientFD){
            clientList.clients[i] = clientList.clients[--clientList.size];
            break;
        }
    }
}

void addClientToClientList(char *name, int clientFD){
    if(clientList.size == clientList.capacity){
        if(clientList.capacity == 0) clientList.capacity = 1;
        clientList.capacity = clientList.capacity * 2;
        clientList.clients = realloc(clientList.clients, sizeof(struct client) * clientList.capacity);
    }
    strcpy(clientList.clients[clientList.size++].name, name);
    clientList.clients[clientList.size-1].clientFD = clientFD;
}