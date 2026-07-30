#pragma once

#include "../../model.h"

extern struct clientList userList;

void manageClientProtocol(struct packetHeader header, int socketFD, char *myName);
void receiveAndPrintMessage(int socketFD, struct packetHeader header);
void receiveUserList(struct packetHeader header, int socketFD, char *myName);
void printUserList(char *myName);
void receiveRoomList(int socketFD, struct packetHeader header);
void handleLeftUser(struct packetHeader, int socketFD);
void removeClientFromUserList(int clientFD);
void addClientToClientList(struct clientList *list, struct client newClient);