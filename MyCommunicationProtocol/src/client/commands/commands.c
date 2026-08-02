#include <string.h>
#include <stdio.h>

#include "commands.h"
#include "../protocol/protocol.h"
#include "../serializer/serializer.h"

void manageCommands(char *commandBuffer, int serverSocketFD, char *myName, int mySocketFD){
    // Main loop commands
    if(strcmp(commandBuffer, "/refresh\n") == 0){
        printf("\n\n");
        printUserList(myName);
        return;
    }

    commandBuffer[strlen(commandBuffer) - 1] = '\0';
    /*
    commands:
        /info - name, description, member list
    */
    
    // Chatting with other users on server (peer to peer communication)
    for(int i = 0; i < userList.size; i++){
        if(strcmp(commandBuffer+1, userList.clients[i].name) == 0){
            free(currentCommunication);
            currentCommunication = malloc(strlen(userList.clients[i].name) + 1);
            strcpy(currentCommunication, userList.clients[i].name);

            printf("-- %s --\n--------------\n", userList.clients[i].name);

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
                packetWriteBytes(&writer, &userList.clients[i].clientFD, sizeof(int));
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
            free(currentCommunication);
            return;
        }
    } 
    // same 'for loop' for group chat
    printf("Invalid input, read '/help'\n");
}