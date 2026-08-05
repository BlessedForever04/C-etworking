#pragma once
#include "../../model.h"
#include <stdlib.h>

extern char *currentCommunication;

void packetWriteBytes(struct packetWriter *writer, void *data, size_t size);
void packetWriterInIt(struct packetWriter *writer, uint32_t payloadSize);
void packetWriteString(struct packetWriter *writer, char *data);

void packetReaderInIt(struct packetReader *reader, uint32_t payloadSize, int socketFD);
char *packetReadString(struct packetReader *reader);
uint8_t *packetReadBytes(struct packetReader *reader, uint32_t payloadSize);