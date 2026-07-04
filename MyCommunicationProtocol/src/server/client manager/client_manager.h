#pragma once
#include "../../model.h"

extern struct clientList clientList;

void removeClientFromClientList(int clientFD);
void addClientToClientList(int clientFD);
void sendClientListToClient(int FD);