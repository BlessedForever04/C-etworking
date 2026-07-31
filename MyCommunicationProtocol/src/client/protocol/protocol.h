#pragma once

#include "../../model.h"

extern struct clientList userList;

void manageClientProtocol(struct packetHeader header, int socketFD, char *myName);
void receiveAndPrintMessage(struct packetHeader header, int socketFD);
void receiveUserList(struct packetHeader header, int socketFD, char *myName);
void printUserList(char *myName);
void receiveRoomList(struct packetHeader header, int socketFD);
void handleLeftUser(struct packetHeader header, int socketFD);
void removeClientFromUserList(int clientFD);
void addClientToClientList(struct clientList *list, struct client newClient);
void addJoinedUserToUserList(struct packetHeader header, int serverSocketFD);