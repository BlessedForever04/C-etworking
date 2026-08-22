#include "../../shared_list/shared_list.h"
#include "../../../shared/serializer/serializer.h"
#include "../../../shared/client_manager/client_manager.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

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

void kickMemberAndShareOnServer(char *myName, char *userName, char *groupName, int serverSocketFD){
    if(strcmp(currentCommunication, "NULL") == 0){
        printf("Open group to kick member, command cannot be executed outside group.\n");
    }
    else{
        int targetFD; 
        for(size_t i = 0; i < userList.size; i++){
            if(strcmp(userList.clients[i].name, userName) == 0){
                targetFD = userList.clients[i].FD;
                break;
            }
        }
        for(size_t i = 0; i < groupList.size; i++){
            if(strcmp(currentCommunication, groupList.group[i].name) == 0){
                if(strcmp(groupList.group[i].admin.name, myName) == 0){
                    removeClientFromClientList(&groupList.group[i].members, targetFD);
                    // Informing the server about kicked member
                    struct packetHeader header;
                    header.type = PACKET_KICK_GROUP_MEMBER;
                    header.payloadSize = sizeof(int) + // targetFD
                                         sizeof(uint32_t) + strlen(userName) +
                                         sizeof(uint32_t) + strlen(currentCommunication); // group name

                    struct packetWriter writer;
                    packetWriterInIt(&writer, header.payloadSize);
                    packetWriteBytes(&writer, &targetFD, sizeof(int));
                    packetWriteString(&writer, userName);
                    packetWriteString(&writer, groupName);
                    // Sending the header
                    send(serverSocketFD, &header, sizeof(header), 0);
                    send(serverSocketFD, writer.buffer, header.payloadSize, 0);
                    printf("Kicked %s\n", userName);
                }
                else{
                    printf("Error: Only admin can access this command.\n");
                }
                break;
            }
        }                
    }
}
