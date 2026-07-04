#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "../../model.h"
#include "serializer.h"

void packetWriterInIt(struct packetWriter *writer, uint32_t payloadSize){
    writer->size = 0;
    writer->capacity = payloadSize;
    writer->buffer = malloc(writer->capacity);

    if(writer->buffer == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
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

void packetWriteString(struct packetWriter *writer, char *data){
    bool resized = false;
    size_t size = strlen(data);

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
    currentBufferLocation += 4;
    memcpy(currentBufferLocation, data, size);

    writer->size += sizeof(size);
    writer->size += size;
}
