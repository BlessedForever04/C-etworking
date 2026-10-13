#include "../../shared_list/shared_list.h"
#include "../../../shared/serializer/serializer.h"
#include "../../../shared/client_manager/client_manager.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

void sendFile(int serverSocketFD, int destinationFD){
    char *file_name = NULL;
    size_t n = 0;

    printf("Enter file name:");
    if(getline(&file_name, &n, stdin) <= 0){
        free(file_name);
        return;
    }

    file_name[strlen(file_name) - 1] = '\0';
    int file = open(file_name, O_RDONLY);

    if(file < 0){
        printf("Error: File doesn't exist, enter correct name.\n");
        free(file_name);
        return;
    }
    else{
        struct stat st;
        if(fstat(file, &st) < 0) perror("fstat");
        struct packetWriter file_information_buffer;

        //                         destinationFD + length of name   + name              + file size
        packetWriterInIt(&file_information_buffer, (sizeof(int) + sizeof(uint32_t) + strlen(file_name) + sizeof(st.st_size)));
        packetWriteBytes(&file_information_buffer, &destinationFD, sizeof(int));
        packetWriteString(&file_information_buffer, file_name);
        packetWriteBytes(&file_information_buffer, &st.st_size, sizeof(st.st_size));
        
        struct packetHeader header;
        header.type = PACKET_FILE;
        header.payloadSize = file_information_buffer.size;

        send(serverSocketFD, &header, sizeof(header), 0);
        send(serverSocketFD, file_information_buffer.buffer, file_information_buffer.size, 0); 

        off_t remaining = st.st_size;
        char buffer[1024 * 1024];

        while(remaining > 0){
            ssize_t bytesRead = read(file, buffer, sizeof(buffer));
            if(bytesRead <= 0){
                break;
            }
            ssize_t bytesSent = send(serverSocketFD, buffer, (size_t)bytesRead, 0);
            if(bytesSent <= 0){
                break;
            }
            remaining -= bytesSent;
        }
        free(file_information_buffer.buffer);
    }
    close(file);
    free(file_name);
}

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
                    free(writer.buffer);
                }
                else{
                    printf("Error: Only admin can access this command.\n");
                }
                break;
            }
        }                
    }
}
