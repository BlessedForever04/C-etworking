#pragma once
#include "../../model.h"

extern struct clientList clientList;

void removeClientFromClientList(int clientFD);
void addClientToClientList(struct clientList *list, struct client newClient);
void sendClientListToClient(int FD);
void sendRoomListToClient(int clientFD);
struct client getClientName(int socketFD);