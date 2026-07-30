#pragma once

#include "../../model.h"

void manageClientProtocol(struct packetHeader header, int socketFD);
void receiveAndPrintMessage(int socketFD, struct packetHeader header);
void receiveClientList(struct packetHeader header, int socketFD);
void receiveRoomList(int socketFD, struct packetHeader header);
void handleLeftUser(struct packetHeader, int socketFD);
void removeClientFromUserList(int clientFD);
void addClientToClientList(struct clientList *list, struct client newClient);