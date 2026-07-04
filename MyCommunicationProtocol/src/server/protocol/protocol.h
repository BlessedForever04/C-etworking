#pragma once
#include "../../model.h"

void manageServerProtocol(struct packetHeader header, int socketFD);
void manageNotice(void);
void manageBroadcast(int socketFD, struct packetHeader header);