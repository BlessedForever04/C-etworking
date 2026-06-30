#ifndef  NETWORK_MANAGER_H
#define NETWORK MANAGER_H
#include <stdint.h>

int createTCPIpv4Socket();
struct sockaddr_in* createSocketAddress(char *ip_address, uint16_t port);
void *receiveAndBroadcastIncomingData(void *arg);
void *receiveAndPrintDataFromServer(void *arg);
void receivingAndBroadcastIncomingDataOnSaperateThread(int clientFD);
void startAcceptingIncomingConnection(int serverSocketFD);
struct acceptedConnection* acceptIncomingConnection(int serverSocketFD);

#endif