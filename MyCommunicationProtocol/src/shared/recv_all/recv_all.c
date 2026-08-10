#include <stdint.h>
#include <sys/socket.h>
#include "recv_all.h"

ssize_t recvAll(int socketFD, void *buffer, size_t size){
        size_t totalReceived = 0;

        while(totalReceived < size){
                ssize_t byteReceived = recv(socketFD, (uint8_t *)buffer + totalReceived, size - totalReceived, 0);
                if(byteReceived <= 0){
                        return byteReceived;
                }
                totalReceived += (size_t)byteReceived;
        }

        return (ssize_t)totalReceived;
}
