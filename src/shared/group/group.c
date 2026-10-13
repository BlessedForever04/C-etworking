#include <stdio.h>
#include <string.h>

#include "../client_manager/client_manager.h"
#include "../model.h"
#include "group.h"

struct group createGroup(int adminFD, char *adminName){           
    char *groupName = NULL;
    char *groupDescription = NULL;
    size_t lineptr = 0;

    printf("Enter group's name: ");
    size_t readByte = getline(&groupName, &lineptr, stdin);

    if(readByte == (size_t)-1){
        perror("getline");
        free(groupName);
        exit(EXIT_FAILURE);
    }

    printf("Enter group's description: ");
    readByte = getline(&groupDescription, &lineptr, stdin);
    
    if(readByte == (size_t)-1){
        perror("getline");
        free(groupName);
        free(groupDescription);
        exit(EXIT_FAILURE);
    }

    groupName[strcspn(groupName, "\n")] = '\0';
    groupDescription[strcspn(groupDescription, "\n")] = '\0';

    struct group newGroup = {0};
    newGroup.name = malloc(strlen(groupName) + 1);
    newGroup.description = malloc(strlen(groupDescription) + 1);
    newGroup.admin.name = malloc(strlen(adminName) + 1);
    newGroup.members = (struct clientList){NULL, 0, 0};

    struct client admin = {adminName, adminFD};
    addClientInClientList(&newGroup.members, admin);
    
    memcpy(newGroup.name, groupName, strlen(groupName) + 1);
    memcpy(newGroup.description, groupDescription, strlen(groupDescription) + 1);
    memcpy(newGroup.admin.name, adminName, strlen(adminName) + 1);
    newGroup.admin.FD = adminFD;

    free(groupName);
    free(groupDescription);
    return newGroup;
}

void addGroupInGroupList(struct groupList *groupList, struct group newGroup){
    if(groupList->size == groupList->capacity){
        if(groupList->capacity == 0) groupList->capacity = 1;
        groupList->capacity *= 2;

        struct group *temp = realloc(groupList->group, sizeof(struct group) * groupList->capacity);
        if(temp == NULL){
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        groupList->group = temp;
    }

    groupList->group[groupList->size].name = malloc(strlen(newGroup.name) + 1);
    memcpy(groupList->group[groupList->size].name, newGroup.name, strlen(newGroup.name) + 1);
    groupList->group[groupList->size].description = malloc(strlen(newGroup.description) + 1);
    memcpy(groupList->group[groupList->size].description, newGroup.description, strlen(newGroup.description) + 1);
    groupList->group[groupList->size].admin.name = malloc(strlen(newGroup.admin.name) + 1);
    memcpy(groupList->group[groupList->size].admin.name, newGroup.admin.name, strlen(newGroup.admin.name) + 1);
    groupList->group[groupList->size].admin.FD = newGroup.admin.FD;
    groupList->group[groupList->size].members = (struct clientList){NULL, 0, 0};

    for(size_t i = 0; i < newGroup.members.size; i++){
        addClientInClientList(&groupList->group[groupList->size].members, newGroup.members.clients[i]);
    }

    groupList->size++;
}

void removeGroupFromGroupList(struct groupList *groupList, char *groupName){
    for(size_t i = 0; i < groupList->size; i++){
        if(strcmp(groupName, groupList->group[i].name) == 0){
            size_t lastIndex = groupList->size - 1;
            free(groupList->group[i].name);
            free(groupList->group[i].description);
            free(groupList->group[i].admin.name);
            freeClientList(&groupList->group[i].members);

            if(i != lastIndex){
                groupList->group[i] = groupList->group[lastIndex];
            }
            groupList->size--;
            break;
        }
    }
}

void freeGroupList(struct groupList *groupList){
    for(size_t i = 0; i < groupList->size; i++){
        free(groupList->group[i].name);
        free(groupList->group[i].description);
        free(groupList->group[i].admin.name);
        freeClientList(&groupList->group[i].members);
    }
    free(groupList->group);
    groupList->group = NULL;
    groupList->size = 0;
    groupList->capacity = 0;
}

void printGroupList(struct groupList groupList){
    printf("-- List of groups --\n--------------------\n");
    int index = 1;
    for(size_t i = 0; i < groupList.size; i++){
        printf("%d: %s\nDescription: %s\n", index++, groupList.group[i].name, groupList.group[i].description);
    }
    if(index == 1){
        printf("No groups formed yet!\n");
    }
    printf("--------------------\n");
}
