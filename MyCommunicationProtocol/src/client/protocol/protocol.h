#pragma once

#include "../../shared/model.h"

extern struct clientList userList;

void manageClientProtocol(struct packetHeader header, int socketFD);
void receiveAndPrintMessage(struct packetHeader header, int socketFD);
void receiveUserList(struct packetHeader header, int socketFD);
void printUserList(char *myName);
void receiveRoomList(struct packetHeader header, int socketFD);
void handleLeftUser(struct packetHeader header, int socketFD);
void removeClientFromUserList(int clientFD);
void addUserInUserList(struct clientList *list, struct client newClient);
void addJoinedUserInUserList(struct packetHeader header, int serverSocketFD);
void manageNewGroup(struct packetHeader header, int serverSocketFD);
void shareNewGroupDetailsToServer(struct group newGroup, int serverSocketFD);
void manageNewMemberInGroup(struct packetHeader header, int serverSocketFD);
