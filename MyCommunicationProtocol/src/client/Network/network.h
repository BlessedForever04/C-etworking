#pragma once

#include <stdint.h>

int createTCPIpv4Socket(void);
struct sockaddr_in* createSocketAddress(char *ip_address, uint16_t port);
void *receiveDataFromServer(void *arg);