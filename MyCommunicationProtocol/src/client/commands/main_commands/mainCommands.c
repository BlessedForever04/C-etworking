#include <stdio.h>
#include <string.h>

#include "../command_manager/commandManager.h"
#include "mainCommands.h"
#include "../../../shared/model.h"
#include "../../../shared/serializer/serializer.h"
#include "../../protocol/protocol.h"
#include "../../shared_list/shared_list.h"

void chatWithUser(struct client *user, char *myName, int mySocketFD, int serverSocketFD){
    free(currentCommunication);
    currentCommunication = malloc(strlen(user->name) + 1);
    strcpy(currentCommunication, user->name);

    printf("-- %s --\n--------------\n", user->name);

    struct messagePacket message;
    size_t lineSize = 0;

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
        
        //                     destinationFD + sourceFD    + nameLen     + myName         + msglen      + msg
        uint32_t payloadSize = sizeof(int)   + sizeof(int) + sizeof(int) + strlen(myName) + sizeof(int) + strlen(message.message);
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