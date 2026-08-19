#pragma once
#include "../../shared/model.h"

extern struct clientList clientList;

void removeClientFromClientList(int clientFD);
void addClientInClientList(struct clientList *list, struct client newClient);
void sendClientListToClient(int FD);
void introduceNewClient(struct client newClient);
struct client getClientName(struct packetHeader header, int socketFD);
