#include <stdio.h>
#include <string.h>

#include "../command_manager/commandManager.h"
#include "mainCommands.h"
#include "../../../shared/model.h"
#include "../../../shared/serializer/serializer.h"
#include "../../protocol/protocol.h"
#include "../../shared_list/shared_list.h"

size_t lineSize = 0;

void chatWithUser(struct client *user, char *myName, int mySocketFD, int serverSocketFD){
    free(currentCommunication);
    currentCommunication = malloc(strlen(user->name) + 1);
    strcpy(currentCommunication, user->name);

    printf("-- %s --\n--------------\n", user->name);

    struct messagePacket message;

    // Communication loop with user
    while(true){
        // Client writing message
        int charCount = getline(&message.message, &lineSize, stdin);
        if(charCount <= 0){
            perror("getline");
            exit(EXIT_FAILURE);
        }
        
        struct packetHeader header = {0};
        if(strcmp(message.message, "/back\n") == 0){
            break;
        }
        
        // Handles internal commands
        if(message.message[0] == '/'){
            manageCommands(message.message, serverSocketFD, myName, mySocketFD);
            continue;
        }
        
        // destinationFD + sourceFD + nameLen + myName + msglen + msg
        uint32_t payloadSize = sizeof(int) +
                               sizeof(int) + 
                               sizeof(uint32_t) + 
                               strlen(myName) + 
                               sizeof(uint32_t) + 
                               strlen(message.message);
        // Building packet header
        header.payloadSize = payloadSize;
        header.type = PACKET_CHAT;
        
        // Serialization (building payload)
        struct packetWriter writer;
        // Buffer initialization
        packetWriterInIt(&writer, payloadSize);
        // DestinationFD
        packetWriteBytes(&writer, &user->FD, sizeof(int));
        // SourceFD
        packetWriteBytes(&writer, &mySocketFD, sizeof(int));
        // Source name
        packetWriteString(&writer, myName);
        // Source message
        packetWriteString(&writer, message.message);
        
        // Sending packet header
        send(serverSocketFD, &header, sizeof(header), 0);
        // Sending message packet
        send(serverSocketFD, writer.buffer, writer.size, 0); 
        free(writer.buffer);
    } 
}

void shareNewGroupDetailsToServer(struct group newGroup, int serverSocketFD){
    // Header creation
    struct packetHeader header;
    header.type = PACKET_CREATE_ROOM;

    // Each string is serialized as uint32 length + payload.
    header.payloadSize = (sizeof(uint32_t) + strlen(newGroup.name)) +
                         (sizeof(uint32_t) + strlen(newGroup.description)) +
                         (sizeof(uint32_t) + strlen(newGroup.admin.name)) +
                         sizeof(newGroup.admin.FD);

    // Buffer writing
    struct packetWriter writer;
    packetWriterInIt(&writer, header.payloadSize);

    packetWriteString(&writer, newGroup.name); // Group name
    packetWriteString(&writer, newGroup.description); // Group description
    packetWriteString(&writer, newGroup.admin.name);
    packetWriteBytes(&writer, &newGroup.admin.FD, sizeof(int));

    // Sending header
    send(serverSocketFD, &header, sizeof(header), 0);
    // Sending buffer
    send(serverSocketFD, writer.buffer, header.payloadSize, 0);

    // Freeing memory
    free(newGroup.admin.name);
    free(newGroup.name);
    free(newGroup.description);
    free(writer.buffer);
}

void getUserAndGroupName(char *userName, char *groupName){
    printf("Enter user name: ");
    int charCount = getline(&userName, &lineSize, stdin);
    if(charCount <= 0){
        perror("getline");
        exit(EXIT_FAILURE);
    }
    lineSize = 0;

    printf("Enter group name: ");
    charCount = getline(&groupName, &lineSize, stdin);
    if(charCount <= 0){
        perror("getline");
        exit(EXIT_FAILURE);
    }
    lineSize = 0;
}

void shareAddedMemberDetailsToServer(char *groupName, struct client newMember, int serverSocketFD){
    struct packetWriter writer;
    struct packetHeader header;

    header.type = PACKET_JOIN_ROOM;
    // User + group
    // User name + FD   + group name
    header.payloadSize = sizeof(uint32_t) + strlen(newMember.name) + sizeof(int) + sizeof(uint32_t) + sizeof(groupName);
    packetWriterInIt(&writer, header.payloadSize);

    packetWriteString(&writer, newMember.name);
    packetWriteBytes(&writer, &newMember.FD, sizeof(int));
    packetWriteString(&writer, groupName);

    //sending header
    send(serverSocketFD, &header, sizeof(header), 0);
    //sending actual buffer
    send(serverSocketFD, writer.buffer, header.payloadSize, 0);
}
