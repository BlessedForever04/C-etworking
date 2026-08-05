#include <stdio.h>
#include <string.h>

#include "../model.h"
#include "group.h"

struct group createGroup(int adminFD, char *adminName){           
    char *groupName = NULL;
    char *groupDescription = NULL;
    size_t lineptr = 0;

    size_t readByte = getline(&groupName, &lineptr, stdin);

    if(readByte){
        perror("getline");
        exit(EXIT_FAILURE);
    }
    else readByte = 0;

    readByte = getline(&groupDescription, &lineptr, stdin);
    
    if(readByte){
        perror("getline");
        exit(EXIT_FAILURE);
    }

    struct group newGroup;
    newGroup.name = malloc(strlen(groupName));
    newGroup.description = malloc(strlen(groupDescription));
    newGroup.admin.name = malloc(strlen(adminName));
    
    memcpy(newGroup.name, groupName, strlen(groupName)); 
    memcpy(newGroup.name, groupDescription, strlen(groupDescription));
    memcpy(newGroup.admin.name, adminName, strlen(adminName));
    newGroup.admin.FD = adminFD;

    return newGroup;
}

void addGroupInGroupList(struct groupList groupList, struct group newGroup){
    if(groupList.size == groupList.capacity){
        if(groupList.capacity == 0) groupList.capacity = 1;
        groupList.capacity *= 2;

        struct group *temp = realloc(groupList.group, sizeof(struct group) * groupList.capacity);
        if(temp == NULL){
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        groupList.group = temp;
    }

    groupList.group[groupList.size].admin.FD = newGroup.admin.FD;

    groupList.group[groupList.size].admin.name = malloc(strlen(newGroup.admin.name));
    memcpy(groupList.group[groupList.size].admin.name, newGroup.admin.name, strlen(newGroup.admin.name));
    groupList.group[groupList.size].name = malloc(strlen(newGroup.name));
    memcpy(groupList.group[groupList.size].name, newGroup.name, strlen(newGroup.name));
    groupList.group[groupList.size].description = malloc(strlen(newGroup.description));
    memcpy(groupList.group[groupList.size].description, newGroup.description, strlen(newGroup.description));

    groupList.size++;
}

void removeGroupFromGroupList(struct groupList *groupList, char *groupName){
    for(int i = 0; i < groupList->size; i++){
        if(strcmp(groupName, groupList->group[i].name) == 0){
            groupList->group[i] = groupList->group[groupList->size - 1];
            free(groupList->group[groupList->size - 1].name);
            free(groupList->group[groupList->size - 1].description);
            free(groupList->group[groupList->size - 1].admin.name);

            for(int j = 0; j < groupList->group[groupList->size - 1].members.size; j++){
                free(groupList->group[groupList->size - 1].members.clients[j].name);
            }

            groupList->size--;
        }
    }
}

void printGroupList(struct groupList groupList){
    printf("-- List of groups --\n--------------------\n");
    int index = 1;
    for(int i = 0; i < groupList.size; i++){
        printf("%d: %s\nDescription: %s\n", index++, groupList.group[i].name, groupList.group[i].description);
    }
    if(index == 1){
        pritnf("No groups formed yet!\n");
    }
    printf("--------------------\n");
}