#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include "../../model.h"

extern struct clientList clientList;

void removeClientFromClientList(int clientFD);
void addClientToClientList(char *name, int clientFD);

#endif