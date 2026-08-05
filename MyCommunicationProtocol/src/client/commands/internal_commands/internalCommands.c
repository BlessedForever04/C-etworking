#include <string.h>
#include <sys/socket.h>

#include "internalCommands.h"
#include "../../../shared/serializer/serializer.h"
#include "../../../shared/model.h"

void shareNewGroupDetailsToServer(struct group newGroup, int serverSocketFD){
    // Header creation
    struct packetHeader header;
    header.type = PACKET_CREATE_ROOM;

    // Format:            sizeof(strlen) + group_name            + sizeof(strlen) + group_description            + sizeof(strlen) + admin_name                  + admin_fd
    header.payloadSize = (sizeof(int)    + strlen(newGroup.name) + sizeof(int)    + strlen(newGroup.description) + sizeof(int)    + strlen(newGroup.admin.name) + sizeof(newGroup.admin.FD));

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