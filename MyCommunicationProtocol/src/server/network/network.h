#pragma once
#include <stdint.h>

int createTCPIpv4Socket();
struct sockaddr_in* createSocketAddress(char *ip_address, uint16_t port);
void *receiveDataFromClient(void *arg);
void *receiveDataFromServer(void *arg);
void receivingAndBroadcastIncomingDataOnSaperateThread(int clientFD);
void startAcceptingIncomingConnection(int serverSocketFD);
struct acceptedConnection* acceptIncomingConnection(int serverSocketFD);