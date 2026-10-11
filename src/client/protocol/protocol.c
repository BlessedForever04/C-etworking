#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "../../shared/model.h"
#include "../../shared/serializer/serializer.h"
#include "../../shared/recv_all/recv_all.h"
#include "../../shared/group/group.h"
#include "../../shared/client_manager/client_manager.h"
#include "../shared_list/shared_list.h"

void manageClientProtocol(struct packetHeader header, int serverSocketFD){
    switch (header.type){
        case PACKET_CLIENT_LIST:
        receiveUserList(header, serverSocketFD);
        break;
        
        case PACKET_GROUP_LIST:
        receiveGroupList(header, serverSocketFD);
        break;
        
        case PACKET_CHAT:
        receiveAndPrintMessage(header, serverSocketFD);
        break;

        case PACKET_USER_LEFT:
        handleLeftUser(header, serverSocketFD);
        break;

        case PACKET_USER_JOINED_SERVER:
        addJoinedUserInUserList(header, serverSocketFD);
        break;
        
        case PACKET_CREATE_ROOM:
        manageNewGroup(header, serverSocketFD);
        break;

        case PACKET_JOIN_ROOM:
        manageNewMemberInGroup(header, serverSocketFD); 
        break;

        case PACKET_GROUP_CHAT:
        manageGroupChat(header, serverSocketFD);
        break;

        case PACKET_KICK_GROUP_MEMBER:
        manageKickedMember(header, serverSocketFD);
        break;

        case PACKET_KICKED:
        handleDepression(header, serverSocketFD);
        break;

        case PACKET_LEAVE_ROOM:
        manageLeftGroupMember(header, serverSocketFD);
        break;

        case PACKET_FILE:
        manageIncomingFile(header, serverSocketFD);
        break;

        default:
        handleItBro();
        // Default case handling
        break;
    }
} 

void manageIncomingFile(struct packetHeader header, int serverSocketFD){
    struct packetReader file_information_buffer;
    packetReaderInIt(&file_information_buffer, header.payloadSize, serverSocketFD);

    // Header.payLoadSize = destinationFD + strlen(file_name) + file_name + st.st_size : file_size
    uint8_t *destinationFD = packetReadBytes(&file_information_buffer, sizeof(int));
    char *file_name = packetReadString(&file_information_buffer);
    uint8_t *temp = packetReadBytes(&file_information_buffer, sizeof(off_t));
    off_t file_size;
    memcpy(&file_size, temp, sizeof(off_t));
 
    // Here open is declared as :  int open(const char *path, int flags, ... mode_t mode);
    // Here flags and mode is just a number which represents flag value and mode
    // Read man 2 open, mode section to know more, 0644 is no magic number XD
    int fileFD = open(file_name, O_WRONLY | O_CREAT, 0644);

    int buffer_size = 1024 * 1024;
    char buffer[buffer_size];

    while(file_size > 0){
        size_t want = file_size < buffer_size ? file_size : buffer_size;
        size_t receivedBytes = recv(serverSocketFD, buffer, want, 0);
        printf("received Bytes: %ld", receivedBytes);
        if(receivedBytes <= 0){
            printf("Error: received 0 bytes during ftp\n");
            return;
        }
        file_size -= receivedBytes;
        write(fileFD, buffer, receivedBytes);
    }

    close(fileFD);
    free(file_information_buffer.buffer);
    free(file_name);
    free(destinationFD);
    free(temp);
}

void manageLeftGroupMember(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);
    uint8_t *FD = packetReadBytes(&reader, sizeof(int));
    int targetFD;
    memcpy(&targetFD, FD, sizeof(int));

    char *userName = packetReadString(&reader);
    char *groupName = packetReadString(&reader);

    for(size_t i = 0; i < groupList.size; i++){
        if(strcmp(groupList.group[i].name, groupName) == 0){
            removeClientFromClientList(&groupList.group[i].members, targetFD);
            if(strcmp(groupList.group[i].name, currentCommunication) == 0){
                printf("%s left the chat!\n", userName);
            }
            break;
        }
    }
}

void handleItBro(){
    printf("What the dog going??\n");
}

void handleDepression(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);
 
    char *groupName = packetReadString(&reader);
    removeGroupFromGroupList(&groupList, groupName);    
    printf("You are kicked by admin!\n");

    char *temp = "NULL\0";
    free(currentCommunication);
    currentCommunication = malloc(strlen(temp) + 1);
    strcpy(currentCommunication, temp);
    free(reader.buffer);
    free(groupName);
}

void manageKickedMember(struct packetHeader header, int socketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, socketFD);
    uint8_t *FD = packetReadBytes(&reader, sizeof(int));  
    int targetFD;
    memcpy(&targetFD, FD, sizeof(int));
    char *userName = packetReadString(&reader);
    char *groupName = packetReadString(&reader);

    for(size_t i = 0; i < groupList.size; i++){
        if(strcmp(groupList.group[i].name, groupName) == 0){
            removeClientFromClientList(&groupList.group[i].members, targetFD);  
            printf("%s got kicked by admin!\n", userName);            
            break;
        }
    }

    free(reader.buffer);
    free(FD);
    free(userName);
    free(groupName);
}

void manageGroupChat(struct packetHeader header, int socketFD){
    struct packetReader reader; 
    packetReaderInIt(&reader, header.payloadSize, socketFD); 
    // grp name, sender name, sent msg
    char *groupName = packetReadString(&reader);
    char *sender = packetReadString(&reader);
    char *message = packetReadString(&reader);

    if(currentCommunication != NULL && strcmp(currentCommunication, groupName) == 0) printf("%s: %s", sender, message);

    free(groupName);
    free(sender);
    free(message);
    free(reader.buffer);
}

void manageNewMemberInGroup(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);

    struct client newMember;
    newMember.name = packetReadString(&reader);
    uint8_t *FD = packetReadBytes(&reader, sizeof(int));
    memcpy(&newMember.FD, FD, sizeof(newMember.FD));
    char *groupName = packetReadString(&reader);

    for(size_t i = 0; i < groupList.size; i++){
        if(strcmp(groupName, groupList.group[i].name) == 0){
            addClientInClientList(&groupList.group[i].members, newMember);
            break;
        }
    }

    free(newMember.name);
    free(groupName);
    free(FD);
    free(reader.buffer);
}

void manageNewGroup(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);
    
    struct group newGroup = {0};
    newGroup.name = packetReadString(&reader);
    newGroup.description = packetReadString(&reader);
    newGroup.admin.name = packetReadString(&reader);
    newGroup.admin.FD = *packetReadBytes(&reader, sizeof(int));

    addClientInClientList(&newGroup.members, newGroup.admin);
    addGroupInGroupList(&groupList, newGroup);

    // freeing memory
    free(newGroup.name);
    free(newGroup.description);
    free(newGroup.admin.name);
    free(reader.buffer);
}

void addJoinedUserInUserList(struct packetHeader header, int serverSocketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, serverSocketFD);
    struct client newUser;
    uint8_t *fd = packetReadBytes(&reader, sizeof(int));
    memcpy(&newUser.FD, fd, sizeof(newUser.FD));
    newUser.name = packetReadString(&reader);

    addClientInClientList(&userList, newUser);
    free(reader.buffer);
    free(fd);
    free(newUser.name);
}

void handleLeftUser(struct packetHeader header, int serverSocketFD){
    int *leftUser = malloc(sizeof(int));
    
    size_t receivedBytes = recvAll(serverSocketFD, leftUser, header.payloadSize);

    if(receivedBytes <= 0){
        perror("recv");
        exit(EXIT_FAILURE);
    }
    removeClientFromClientList(&userList, *leftUser);
    free(leftUser);
}

void receiveAndPrintMessage(struct packetHeader header, int socketFD){
    struct packetReader reader;
    
    packetReaderInIt(&reader, header.payloadSize, socketFD);

    // Just have to receive it
    uint8_t *tempFD = packetReadBytes(&reader, sizeof(int)); // Receiving destinationFD
    char *sender = packetReadString(&reader); // Sender's name
    char *message = packetReadString(&reader); // Sender's message
        
    if(currentCommunication != NULL && strcmp(currentCommunication, sender) == 0) printf("%s: %s", sender, message);

    free(tempFD);
    free(sender);
    free(message);
    free(reader.buffer);
}

void receiveUserList(struct packetHeader header, int socketFD){
    // Receiving the users (clientList)
    struct packetReader reader; 
    packetReaderInIt(&reader, header.payloadSize, socketFD);

    struct client user;
    while(reader.size > 0){
        user.name = packetReadString(&reader);
        uint8_t *fd = packetReadBytes(&reader, sizeof(int));
        memcpy(&user.FD, fd, sizeof(user.FD));
        addClientInClientList(&userList, user);
        free(user.name);
        free(fd);
    }
    
    free(reader.buffer); 
}

void printUserList(char *myName){
    printf("List of connected users:\n------------------------\n");
    int index = 1;
    for(size_t i = 0; i < userList.size; i++){
        if(strcmp(myName, userList.clients[i].name) == 0) continue;
        printf("%d: %s\n", index, userList.clients[i].name);
        index++;
    }
    // }
    printf("------------------------\n");
}

void receiveGroupList(struct packetHeader header, int socketFD){
    struct packetReader reader;
    packetReaderInIt(&reader, header.payloadSize, socketFD);

    struct group newGroup = {0};
    newGroup.name = packetReadString(&reader);
    newGroup.description = packetReadString(&reader);
    newGroup.admin.name = packetReadString(&reader);
    uint8_t *FD = packetReadBytes(&reader, sizeof(int));
    memcpy(&newGroup.admin.FD, FD, sizeof(int));
    
    struct client groupMember;
    while(reader.size > 0){
        groupMember.name = packetReadString(&reader);
        FD = packetReadBytes(&reader, sizeof(int));
        memcpy(&groupMember.FD, FD, sizeof(int));

        addClientInClientList(&newGroup.members, groupMember);
        free(groupMember.name);
        free(FD);
    }

    addGroupInGroupList(&groupList, newGroup);

    free(newGroup.name);
    free(newGroup.description);
    free(newGroup.admin.name);

    for(size_t i = 0; i < newGroup.members.size; i++){
        free(newGroup.members.clients[i].name);
    }
}
