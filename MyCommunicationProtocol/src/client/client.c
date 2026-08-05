#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#include "commands/command_manager/commandManager.h"
#include "../shared/serializer/serializer.h"
#include "protocol/protocol.h"
#include "network/network.h"
#include "../shared/model.h"

int main(){
    uint16_t port = 5000;

    // Client's name
    char *myName = NULL;
    printf("Enter your name: ");
    size_t lineSize = 0;
    ssize_t charCount = getline(&myName, &lineSize, stdin);
    
    if(charCount == -1){
        perror("getline");
        free(myName);
        return 1;
    }
    
    myName[charCount-1] = '\0';
    lineSize = 0;

    // Socket and address creation
    int socketFD = createTCPIpv4Socket(); 
    struct sockaddr_in *serverAddressPtr = createSocketAddress("127.0.0.1", port); 
    
    // Connecting to server socket
    int connectStatus = connect(socketFD, (struct sockaddr*)serverAddressPtr, sizeof(*serverAddressPtr)); 
    if(connectStatus == 0) printf("Connection successful!\n");
    if(connectStatus != 0){
        perror("connect");
        return 1;
    }

    // Introduction header
    struct packetHeader introHeader;
    introHeader.type = PACKET_INTRO;
    introHeader.payloadSize = strlen(myName) + 1;

    // Can't pass multiple variables via function used for pthread so coupling name and socketFD
    struct pair socketAndName;
    socketAndName.name = malloc(strlen(myName) + 1);
    strcpy(socketAndName.name, myName);
    socketAndName.socketFD = socketFD;

    // Thread for receiving data from server and printing on terminal
    pthread_t receiveThread;
    pthread_create(&receiveThread, NULL, receiveDataFromServer, &socketAndName);
    int mySocketFD = getMySocketFD(myName);

    // Sending header for name
    send(socketFD, &introHeader, sizeof(introHeader), 0);
    
    // Sending client name to server (Client is added in the clients list)
    send(socketFD, myName, introHeader.payloadSize, 0);    
    
    // Writing message to server
    struct messagePacket message;
    message.sender = malloc(strlen(myName) + 1);
    snprintf(message.sender, strlen(myName) + 1, "%s", myName);

    // Main loop
    while(1){
        // Client writing message in main external loop
        charCount = getline(&message.message, &lineSize, stdin);
        
        struct packetHeader header = {0};
        if(strcmp(message.message, "/quit\n") == 0 || strcmp(message.message, "/q\n") == 0){
            header.payloadSize = 0;
            header.type = PACKET_USER_LEFT;
            send(socketFD, &header, sizeof(header), 0); // Header
            break;
        }

        if(message.message[0] == '/'){
            manageCommands(message.message, socketFD, myName, mySocketFD);
            continue;
        }
        else{
          printf("Invalid input, read '/help'\n");
        }
    }

    free(myName);
    free(message.message);
    free(message.sender);
    close(socketFD);
    return 0;
}