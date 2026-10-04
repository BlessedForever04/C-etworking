#pragma once

void sendData(int destinationFD);
void printGroupInformation(char *groupName);
void kickMemberAndShareOnServer(char *myName, char *userName, char *groupName, int serverSocketFD);
