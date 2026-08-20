#pragma once

#include "../../shared/model.h"

extern struct clientList userList;

void manageClientProtocol(struct packetHeader header, int socketFD);
void receiveAndPrintMessage(struct packetHeader header, int socketFD);
void receiveUserList(struct packetHeader header, int socketFD);
void printUserList(char *myName);
void receiveGroupList(struct packetHeader header, int socketFD);
void handleLeftUser(struct packetHeader header, int socketFD);
void removeUserFromUserList(int clientFD);
void addJoinedUserInUserList(struct packetHeader header, int serverSocketFD);
void manageNewGroup(struct packetHeader header, int serverSocketFD);
void shareNewGroupDetailsToServer(struct group newGroup, int serverSocketFD);
void manageNewMemberInGroup(struct packetHeader header, int serverSocketFD);
void manageGroupChat(struct packetHeader header, int serverSocketFD);
