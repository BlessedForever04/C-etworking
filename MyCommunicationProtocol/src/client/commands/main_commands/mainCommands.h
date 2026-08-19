#pragma once

#include "../../../shared/model.h"

void chatWithUser(struct client *user, char *myName, int mySocketFD, int serverSocketFD);
void shareNewGroupDetailsToServer(struct group newGroup, int serverSocketFD);
void getUserAndGroupName(char *userName, char *groupName);
void shareAddedMemberDetailsToServer(char *groupName, struct client newMember, int serverSocketFD);
