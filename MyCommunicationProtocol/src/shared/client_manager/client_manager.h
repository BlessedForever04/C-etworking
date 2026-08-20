#pragma once
#include "../../shared/model.h"

extern struct clientList clientList;

void removeClientFromClientList(struct clientList *clientList, int clientFD);
void addClientInClientList(struct clientList *clientList, struct client newClient);
void sendClientListToClient(struct clientList clientList, int FD);
void introduceNewClient(struct clientList clientList, struct client newClient);
struct client getClientName(struct packetHeader header, int socketFD);
