#pragma once

#include "../model.h"

struct group createGroup(int adminFD, char *adminName);
void addGroupInGroupList(struct groupList groupList, struct group newGroup);
void removeGroupFromGroupList(struct groupList *groupList, char *groupName);