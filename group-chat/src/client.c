#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "util.h"
#include <string.h>

int main(){
    uint16_t port = 5000;
    int clientSocketFD = createTCPIpv4Socket(); 
    struct sockaddr_in *serverAddressPtr = createSocketAddress("127.0.0.1", port); 

    int connectStatus = connect(clientSocketFD, (struct sockaddr*)serverAddressPtr, sizeof(*serverAddressPtr)); 
    if(connectStatus == 0) printf("Connection was successful!\n");
    if(connectStatus != 0){
        perror("connect");
        return 1;
    }

    char *name = NULL;
    size_t lineSize = 0;
    printf("Enter your name: ");
    ssize_t charCount = getline(&name, &lineSize, stdin);
    name[charCount-1] = '\0';
    send(clientSocketFD, name, 50, 0);

    char *message = NULL;
    char output[1024];
    
    pthread_t receiveThread;
    pthread_create(&receiveThread, NULL, receiveAndPrintDataFromServer, &clientSocketFD);
    
    while(1){
        strcpy(output, name);
        strcat(output, ": ");
        charCount = getline(&message, &lineSize, stdin); 
        strcat(output, message);
        send(clientSocketFD, output, 1024, 0);
        output[0] = '\0';
        if(strcmp(message, "bye\n") == 0){
            break;
        }
    }

    printf("User left the chat\n");

    close(clientSocketFD);
    return 0;
}