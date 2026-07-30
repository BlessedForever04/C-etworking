#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "serializer/serializer.h"
#include "network/network.h"
#include "../model.h"

/*
  There are so many messed up things that has to be fixed, 
  Main thing is that we can't transfer a struct directly which includes pointers as properties, 
  For that we have to send each pointer individually with its payload size
  Now this has to be done using sendAll and recvAll implementation

  Have to implement serialization

  Next thing to do is that we have to implement different protocols for server and client
  header management has to be done in this phase


Checklist
Client:
  1. Build packetChat (name, message)
  2. Build payload (strlen(name), name, strlen(message), message)
  3. Build header (packetType, payloadSize)
  4. Send header 
  5. Send payload

Server:
  6. Recv header
  7. Recv Payload
  8. ManageserverProtocol(header.type)

Client:
  9. Recv header
  10. ManageclientProtocol(header.type);
*/

int main(){
    uint16_t port = 5000;

    // Client's name
    char *name = NULL;
    printf("Enter your name: ");
    size_t lineSize = 0;
    ssize_t charCount = getline(&name, &lineSize, stdin);
    
    if(charCount == -1){
        perror("getline");
        free(name);
        return 1;
    }
    
    name[charCount-1] = '\0';
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
    struct packetHeader introHeader;
    introHeader.type = PACKET_INTRO;
    introHeader.payloadSize = strlen(name) + 1;

    // Sending header for name
    send(socketFD, &introHeader, sizeof(introHeader), 0);
    
    // Sending client name to server (Client is added in the clients list)
    send(socketFD, name, introHeader.payloadSize, 0);    

    // Thread for receiving data from server and printing on terminal
    pthread_t receiveThread;
    pthread_create(&receiveThread, NULL, receiveDataFromServer, &socketFD);
    
    // Writing message to server
    struct messagePacket message;
    message.sender = malloc(strlen(name) + 1);
    snprintf(message.sender, strlen(name) + 1, "%s", name);
    while(1){
        // Client writing message
        charCount = getline(&message.message, &lineSize, stdin);
        
        struct packetHeader header = {0};
        if(strcmp(message.message, "bye\n") == 0){
            header.payloadSize = 0;
            header.type = PACKET_USER_LEFT;
            send(socketFD, &header, sizeof(header), 0); // Header
            break;
        }

        uint32_t payloadSize = 4 + strlen(name) + 4 + strlen(message.message);
        // Building packet header
        header.payloadSize = payloadSize;
        header.type = PACKET_CHAT;
        
        // Serialization (building payload)
        struct packetWriter writer;
        packetWriterInIt(&writer, payloadSize);
        packetWriteString(&writer, name);
        packetWriteString(&writer, message.message);

        // Sending packet header
        send(socketFD, &header, sizeof(header), 0);
        // Sending message packet
        send(socketFD, writer.buffer, writer.size, 0); 
    }

    free(name);
    free(message.message);
    free(message.sender);
    close(socketFD);
    return 0;
}