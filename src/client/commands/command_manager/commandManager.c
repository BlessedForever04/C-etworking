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

static void printHelpMenu(void){
    printf("\n========================================\n");
    printf("          Cetworking Help Menu\n");
    printf("========================================\n\n");

    printf("General commands:\n");
    printf("  /help              Show this help menu\n");
    printf("  /help chat         Show chat-mode commands\n");
    printf("  /help group        Show group-chat commands\n");
    printf("  /refresh           Show online users and groups\n");
    printf("  /quit or /q        Exit the application\n\n");

    printf("User chat commands:\n");
    printf("  /chat <username>   Start a private chat with a user\n");
    printf("  /send              Send a file while chatting with someone\n");
    printf("  /back              Exit the current chat and return\n\n");

    printf("Group commands:\n");
    printf("  /create_group      Create a new group room\n");
    printf("  /open <group>      Open an existing group chat\n");
    printf("  /add_member        Add a user to the selected group\n");
    printf("  /leave_group <name> Leave a group\n");
    printf("  /kick <user>       Kick a member from the current group\n");
    printf("  /info              Show information about the current group\n\n");

    printf("Examples:\n");
    printf("  /chat Alice\n");
    printf("  /create_group\n");
    printf("  /open TeamChat\n");
    printf("  /kick Bob\n");
    printf("  /leave_group TeamChat\n\n");
    printf("========================================\n\n");
}

static void printChatHelpMenu(void){
    printf("\n========================================\n");
    printf("          Chat Mode Help\n");
    printf("========================================\n\n");

    printf("While in a private chat:\n");
    printf("  <message>          Send a message to this user\n");
    printf("  /send              Send a file to this user\n");
    printf("  /back              Exit this chat and return\n\n");
    printf("Example:\n");
    printf("  hello there\n");
    printf("  /send\n");
    printf("  /back\n\n");
    printf("========================================\n\n");
}

static void printGroupHelpMenu(void){
    printf("\n========================================\n");
    printf("          Group Chat Help\n");
    printf("========================================\n\n");

    printf("Inside a group:\n");
    printf("  <message>          Send a message to the whole group\n");
    printf("  /info              Show group members and description\n");
    printf("  /kick <user>       Kick a member from the group\n");
    printf("  /back              Leave the group chat and return\n\n");
    printf("Example:\n");
    printf("  hello everyone\n");
    printf("  /info\n");
    printf("  /kick Bob\n");
    printf("  /back\n\n");
    printf("========================================\n\n");
}

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
            printHelpMenu();
            return;
        }

        if(argc == 2){
            if(strcmp(argv[1], "chat") == 0){
                printChatHelpMenu();
                return;
            }
            if(strcmp(argv[1], "group") == 0){
                printGroupHelpMenu();
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
