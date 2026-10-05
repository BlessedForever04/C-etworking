#include "../../shared_list/shared_list.h"
#include "../../../shared/serializer/serializer.h"
#include "../../../shared/client_manager/client_manager.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

void sendFile(int serverSocketFD, int destinationFD){
    char *file_name;
    size_t n = 0;

    printf("Enter file name:");
    getline(&file_name, &n, stdin);

    file_name[strlen(file_name) - 1] = '\0';

    int file = open(file_name, O_RDONLY);

    if(file < 0){
        printf("Error: File doesn't exist, enter correct name.\n");
        return;
    }
    else{
        struct stat st;
        if(fstat(file, &st) < 0) perror("fstat");
        off_t offset = 0;
        struct packetWriter writer;

        //                         length of name   + name              + file size
        packetWriterInIt(&writer, (sizeof(uint32_t) + strlen(file_name) + sizeof(uint32_t)));
        packetWriteBytes(&writer, &destinationFD, sizeof(int));
        packetWriteString(&writer, file_name);
        packetWriteBytes(&writer, &st.st_size, sizeof(st.st_size));
        
        struct packetHeader header;
        header.type = PACKET_FILE;
        header.payloadSize = writer.size;

        send(serverSocketFD, &header, sizeof(header), 0);
        send(serverSocketFD, writer.buffer, writer.size, 0); 

        int remaining = st.st_size;

        printf("Remaining b4: %d", remaining);

        while(remaining > 0){
            ssize_t sent = sendfile(serverSocketFD, file, &offset, remaining);
            remaining -= sent; 
            printf("Remaining ftr: %d", remaining);
        }
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
                }
                else{
                    printf("Error: Only admin can access this command.\n");
                }
                break;
            }
        }                
    }
}
