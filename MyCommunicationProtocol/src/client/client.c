#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "../server/network manager/network_manager.h"
#include "../model.h"

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

    // Socket and address creation
    int clientSocketFD = createTCPIpv4Socket(); 
    struct sockaddr_in *serverAddressPtr = createSocketAddress("127.0.0.1", port); 

    // Connecting to server socket
    int connectStatus = connect(clientSocketFD, (struct sockaddr*)serverAddressPtr, sizeof(*serverAddressPtr)); 
    if(connectStatus == 0) printf("Connection was successful!\n");
    if(connectStatus != 0){
        perror("connect");
        return 1;
    }

    // Sending client name to server
    send(clientSocketFD, name, strlen(name) + 1, 0);

    char *message = NULL;
    
    // Thread for receiving data from server and printing on terminal
    pthread_t receiveThread;
    pthread_create(&receiveThread, NULL, receiveAndPrintDataFromServer, &clientSocketFD);
    
    // Writing message to server
    while(1){
        printf("You: ");
        charCount = getline(&message, &lineSize, stdin);
        struct response response = {0};
        snprintf(response.sender, sizeof(response.sender), "%s", name);
        snprintf(response.message, sizeof(response.message), "%s", message);

        send(clientSocketFD, &response, sizeof(struct response), 0);

        if(strcmp(message, "bye\n") == 0){
            break;
        }
    }

    free(name);
    free(message);
    close(clientSocketFD);
    return 0;
}