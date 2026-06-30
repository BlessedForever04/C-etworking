#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "network manager/network_manager.h"

int main(){
    uint16_t port = 5000;
    // Creating socket and address
    int serverSocketFD = createTCPIpv4Socket();
    struct sockaddr_in *serverAddressPtr = createSocketAddress("127.0.0.1", port);

    // Binding the address to socket (AKA assigning name to socket)
    int bindStatus = bind(serverSocketFD, (struct sockaddr*) serverAddressPtr, sizeof(*serverAddressPtr));
    if(bindStatus == 0) printf("Server binding is successful!\n");
    if(bindStatus < 0){
        perror("bind");
        return 1;
    }

    int listenStatus = listen(serverSocketFD, 5);
    if(listenStatus == 0) printf("Server is listening on port %d..\n", port);
    if(listenStatus != 0){
        perror("listen");
        return 1;
    }

    startAcceptingIncomingConnection(serverSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);
    return 0;
}

// Implement threading