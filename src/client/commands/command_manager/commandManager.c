#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "../main_commands/mainCommands.h"
#include "../internal_commands/internalCommands.h"
#include "../help/help.h"
#include "commandManager.h"
#include "../../../shared/group/group.h"
#include "../../../shared/client_manager/client_manager.h"
#include "../../protocol/protocol.h"
#include "../../shared_list/shared_list.h"

void manageCommands(char *commandBuffer, int serverSocketFD, char *myName, int mySocketFD){
    uint8_t isNotFound = 1;
    char *argv[3];
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

    if(strcmp(argv[0], "/help") == 0){
        if(argc == 1){
            printMainHelp();
            return;
        }

        if(argc == 2){
            if(strcmp(argv[1], "chat") == 0){
                printChatHelp();
                return;
            }
            if(strcmp(argv[1], "group") == 0){
                printGroupHelp();
                return;
            }
            printf("Unknown help topic. Try: /help, /help chat, /help group\n");
            return;
        }

        printf("Usage: /help, /help chat, /help group\n");
        return;
    }

    if(argc == 1){
        if(strcmp(argv[0], "/add_member") == 0){
            char *memberName = NULL;
            char *groupName = NULL;

            getUserAndGroupName(&memberName, &groupName);

            struct client *newMember = NULL;
            for(size_t i = 0; i < userList.size; i++){
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

        if(strcmp(argv[0], "/refresh") == 0){
            printf("\n");
            printUserList(myName);
            printGroupList(groupList);
            return;
        }

        if(strcmp(argv[0], "/create_group") == 0){
            struct group newGroup = createGroup(mySocketFD, myName);
            addGroupInGroupList(&groupList, newGroup);
            shareNewGroupDetailsToServer(newGroup, serverSocketFD);
            return;
        }
    }

    if(argc == 2){
        if(strcmp(argv[0], "/chat") == 0){
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
            for(size_t i = 0; i < groupList.size; i++){
                if(strcmp(argv[1], groupList.group[i].name) == 0){
                    isNotFound = 0;
                    chatWithinGroup(argv[1], myName, mySocketFD, serverSocketFD);
                    break;
                }
            }
            if(isNotFound){
                printf("Error: Group doesn't exist, please enter a valid group name!\n");
            }
            else{
                isNotFound = 1;
            }
            return;
        }

        if(strcmp(argv[0], "/leave_group") == 0){
            leaveGroupAndInformServer(myName, argv[1], serverSocketFD);
            return;
        }

        if(strcmp(argv[0], "/kick") == 0){
            kickMemberAndShareOnServer(myName, argv[1], currentCommunication, serverSocketFD);
            return;
        }
    }

    printf("Invalid command, try /help\n");
}
