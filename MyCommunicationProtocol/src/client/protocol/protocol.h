#pragma once

#include "../../model.h"

void manageClientProtocol(struct packetHeader header, int socketFD);
void manageClientList(int socketFD);