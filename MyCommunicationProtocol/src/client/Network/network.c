#include "network.h"
#include "../../model.h"

void *receiveDataFromServer(void *arg){ // here the function has to be of *function(*void) type to be used for pthread
    int serverSocketFD = *(int*)arg;
    
    struct packetHeader header = {0};

    while(1){
        int byteReceived = recv(serverSocketFD, &header, sizeof(header), 0);
        if(byteReceived > 0){
            manageClientProtocol(header, serverSocketFD);
        }
    }
    return NULL;
}