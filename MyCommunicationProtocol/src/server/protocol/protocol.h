#pragma once
#include "../../model.h"

void manageServerProtocol(struct packetHeader header, int socketFD);
void manageNotice(void);
void manageBroadcast(int socketFD, struct packetHeader header);
void manageLeftClient(int socketFD);
void managePeerChat(struct packetHeader header, int sourceClientFD);
void manageNewClient(struct packetHeader header, int serverSocketFD);