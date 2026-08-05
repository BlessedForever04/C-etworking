#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "model.h"
#include "shared/recv_all.h"
#include "serializer.h"

char *currentCommunication = NULL;

void packetWriterInIt(struct packetWriter *writer, uint32_t payloadSize){
    writer->size = 0;
    writer->capacity = payloadSize;
    writer->buffer = malloc(writer->capacity);

    if(writer->buffer == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

void packetReaderInIt(struct packetReader *reader, uint32_t payloadSize, int socketFD){
    reader->buffer = malloc(payloadSize);
    if(recvAll(socketFD, reader->buffer, payloadSize) <= 0){
        free(reader->buffer);
        reader->buffer = NULL;
        reader->offset = NULL;
        reader->size = 0;
        return;
    }
    reader->offset = reader->buffer;
    reader->size = payloadSize;
}

char *packetReadString(struct packetReader *reader){
    uint32_t stringLength;
    memcpy(&stringLength, reader->offset, sizeof(uint32_t));

    reader->offset += sizeof(uint32_t);
    reader->size -= sizeof(uint32_t);

    char *string = malloc(stringLength + 1);
    if(string == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(string, reader->offset, stringLength);
    string[stringLength] = '\0';

    reader->offset += stringLength;
    reader->size -= stringLength;

    return string;
}

uint8_t *packetReadBytes(struct packetReader *reader, uint32_t payloadSize){ // Here payload is any byte stream
    uint8_t *bytes = malloc(payloadSize);
    memcpy(bytes, reader->offset, payloadSize);
    reader->offset += payloadSize;
    reader->size -= payloadSize;
    return bytes;
}

void packetWriteBytes(struct packetWriter *writer, void *data, size_t size){
    bool resized = false;

    while(writer->size + size >= writer->capacity){
        writer->capacity *= 2;
        resized = true;
    }

    if(resized) {
        writer->buffer = realloc(writer->buffer, writer->capacity);
        if(writer->buffer == NULL){
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }    

    uint8_t *currentBufferLocation = writer->buffer + writer->size;

    memcpy(currentBufferLocation, data, size);
    writer->size += size;
}

/* 
    String in buffer is written as : string len + string
*/
void packetWriteString(struct packetWriter *writer, char *data){
    bool resized = false;
    uint32_t size = (uint32_t)strlen(data);

    while(writer->size + sizeof(size) + size >= writer->capacity){
        writer->capacity *= 2;
        resized = true;
    }

    if(resized) {
        writer->buffer = realloc(writer->buffer, writer->capacity);
        if(writer->buffer == NULL){
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }    
    uint8_t *currentBufferLocation = writer->buffer + writer->size;
    
    memcpy(currentBufferLocation, &size, sizeof(size));
    currentBufferLocation += sizeof(size);
    memcpy(currentBufferLocation, data, size);

    writer->size += sizeof(size); // Size of the string
    writer->size += size;         // Actual string
}
