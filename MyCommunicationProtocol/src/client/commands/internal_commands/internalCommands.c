#include "../../shared_list/shared_list.h"

void printGroupInformation(char *groupName){
    for (size_t i = 0; i < groupList.size; i++){
        if (strcmp(groupName, groupList.group[i].name) == 0){
            printf("--- Group Information ---\n");
            printf("Name: %s\n", groupName);
            printf("Description: %s\n", groupList.group[i].description);
            printf("Members:\n");

            size_t index = 1;
            for (size_t j = 0; j < groupList.group[i].members.size; j++){
                printf("%zu. %s\n", index++, groupList.group[i].members.clients[j].name);
            }
            break;
        }
    }
}