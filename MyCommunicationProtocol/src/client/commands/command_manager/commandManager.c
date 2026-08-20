#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "../main_commands/mainCommands.h"
#include "commandManager.h"
#include "../../../shared/group/group.h"
#include "../../../shared/client_manager/client_manager.h"
#include "../../protocol/protocol.h"
#include "../../shared_list/shared_list.h"

void manageCommands(char *commandBuffer, int serverSocketFD, char *myName, int mySocketFD){
    uint8_t isNotFound = 1;
    // Main loop commands
    char *argv[3]; // Max 3 argv allowed for commands, increase this to scale for more command options
    uint8_t argc = 0;
    char *token = strtok(commandBuffer, " \n");

    while(token != NULL && argc < 3){
        argv[argc++] = token;
        token = strtok(NULL, " \n");
    }

    if(argc == 0) {
        printf("Invalid command, try /help\n");
        return;
    }

    if(argc == 1){
        if(strcmp(argv[0], "/add_member") == 0){
            char *memberName = NULL;
            char *groupName = NULL;

            // Input from user
            getUserAndGroupName(&memberName, &groupName);
            printf("Returned after getting the member name and grp name\n");

            struct client *newMember = NULL;
            for(size_t i = 0; i < userList.size; i++){
                printf("Comparing :%s with %s\n", memberName, userList.clients[i].name);
                if(strcmp(memberName, userList.clients[i].name) == 0){
                    newMember = &userList.clients[i];
                    break;
                }
            }

            if(newMember == NULL){
                printf("Error: User doesn't exist, please enter valid user name!\n");
            }
            else{
                struct group *group = NULL;
                for(size_t i = 0; i < groupList.size; i++){
                    if(strcmp(groupList.group[i].name, groupName) == 0){
                        group = &groupList.group[i];
                        break;
                    }
                }
                if(group == NULL){
                    printf("Error: Group doesn't exist, please enter a valid group name!\n");
                }
                else{
                    addClientInClientList(&group->members, *newMember);
                    shareAddedMemberDetailsToServer(group->name, *newMember, serverSocketFD);
                }
            }
            free(memberName);
            free(groupName);
            return;
        }

        if(strcmp(argv[0], "/help") == 0){
            // print usage
            return;
        }

        if(strcmp(argv[0], "/refresh") == 0){
            printf("\n");
            printUserList(myName);
            printGroupList(groupList);
            return;
        }

        if(strcmp(argv[0], "/create_group") == 0){
            // creation of group
            struct group newGroup = createGroup(mySocketFD, myName);
            addGroupInGroupList(&groupList, newGroup);
            shareNewGroupDetailsToServer(newGroup, serverSocketFD);
            return;
        }
    }

    if(argc == 2){
        if(strcmp(argv[0], "/chat") == 0){
            // Chatting with other users on server (peer to peer communication)
            for(size_t i = 0; i < userList.size; i++){
                if(strcmp(argv[1], userList.clients[i].name) == 0){
                    isNotFound = 0;
                    chatWithUser(&userList.clients[i], myName, mySocketFD, serverSocketFD);
                    break;
                }
            }
            if(isNotFound){
                printf("User not found\n"); 
            }
            else{
                isNotFound = 1;
            }
            return;
        }

        if(strcmp(argv[0], "/open") == 0){
            // Chatting within a group
            for(size_t i = 0; i < groupList.size; i++){
                if(strcmp(argv[1], groupList.group[i].name) == 0){
                    isNotFound = 0;
                    chatWithinGroup(argv[1], myName, mySocketFD, serverSocketFD);
                }
            }
        } 

        if(strcmp(argv[1], "delete_group") == 0){
            // implement deletion of group
            return;
        }
    }

    printf("Invalid command, try /help\n");
}
