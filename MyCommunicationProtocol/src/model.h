#ifndef MODEL_H
#define MODEL_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    // Chat
    PACKET_CHAT, // Communication packet for peer to peer communication (sender, receiver, data)
    PACKET_WHISPER, // Whispering inside group chat
    PACKET_GROUP_CHAT, // Message to a group

    // Client List and Room list
    PACKET_CLIENT_LIST,
    PACKET_ROOM_LIST,

    PACKET_INTRO, // Only for sending name's payload size while accepting the client

    // User events
    PACKET_USER_JOINED_SERVER,
    PACKET_USER_JOINED, // Header + name
    PACKET_USER_LEFT, // Header

    // Rooms / Channels / GROUPS
    PACKET_CREATE_ROOM, // Room name, description and password
    PACKET_DELETE_ROOM, // Room name and password
    PACKET_JOIN_ROOM, // Room name and password
    PACKET_LEAVE_ROOM, // Only header
    PACKET_ROOM_INFO, // Room name
    
    // Administration
    PACKET_KICK, // Username
    PACKET_MUTE, // Only header
    PACKET_UNMUTE, // Only header

    // File
    PACKET_FILE, // Header + (uint32_t filenameSize + char *filename + uint32_t dataSize + uint8_t *data)

    // VOICE CHAT
    PACKET_VOICE_JOIN, 
    PACKET_VOICE_LEAVE,
    PACKET_VOICE_MUTE,
    PACKET_VOICE_UNMUTE,

} packetType;

struct packetRoomCreation{
    char *roomName;
    char *password;
    uint8_t roomCapacity;
};

struct packetWriter{
    uint8_t *buffer;
    uint32_t size;
    uint32_t capacity;
};

struct pair{
    int socketFD;
    char *name;
};

struct packetReader{
    uint8_t *buffer;
    uint32_t size;
    uint8_t *offset;
};

struct packetHeader{
    packetType type;
    uint32_t payloadSize;
};

// save packet is used for broadcasting only header is different
struct messagePacket{
    char *sender;
    char *message;
};


struct acceptedConnection{
    int FD;
    struct sockaddr_in socketAddress;
    bool acceptedSuccessfuly;
    int error;
};

struct clientList{    
    struct client *clients;
    int size;
    int capacity;
};

struct client{
    char *name;
    int FD;
};
struct groupList{
    struct group *group;
    size_t size;
    size_t capacity;
};

struct group{
    char *name;
    char *description;
    struct client admin;
    struct clientList members;
};

#endif