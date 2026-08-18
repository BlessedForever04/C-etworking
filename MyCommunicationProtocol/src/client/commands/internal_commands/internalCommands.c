#include <string.h>
#include <sys/socket.h>

#include "internalCommands.h"
#include "../../../shared/serializer/serializer.h"
#include "../../../shared/model.h"

void shareNewGroupDetailsToServer(struct group newGroup, int serverSocketFD){
    // Header creation
    struct packetHeader header;
    header.type = PACKET_CREATE_ROOM;

    // Each string is serialized as uint32 length + payload.
    header.payloadSize = (sizeof(uint32_t) + strlen(newGroup.name)) +
                         (sizeof(uint32_t) + strlen(newGroup.description)) +
                         (sizeof(uint32_t) + strlen(newGroup.admin.name)) +
                         sizeof(newGroup.admin.FD);

    // Buffer writing
    struct packetWriter writer;
    packetWriterInIt(&writer, header.payloadSize);

    packetWriteString(&writer, newGroup.name); // Group name
    packetWriteString(&writer, newGroup.description); // Group description
    packetWriteString(&writer, newGroup.admin.name);
    packetWriteBytes(&writer, &newGroup.admin.FD, sizeof(int));

    // Sending header
    send(serverSocketFD, &header, sizeof(header), 0);
    // Sending buffer
    send(serverSocketFD, writer.buffer, header.payloadSize, 0);

    // Freeing memory
    free(newGroup.admin.name);
    free(newGroup.name);
    free(newGroup.description);
    free(writer.buffer);
}