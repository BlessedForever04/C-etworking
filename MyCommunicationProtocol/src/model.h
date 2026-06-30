#ifndef MODEL_H
#define MODEL_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    // Auth
    PACKET_LOGIN,
    PACKET_LOGIN_SUCCESS,
    PACKET_LOGIN_FAILED,
    PACKET_LOGOUT,
    PACKET_DISCONNECT,

    // cHAT
    PACKET_CHAT,
    PACKET_PRIVATE_MESSAGE,
    PACKET_BROADCAST,

    // User events
    PACKET_USER_JOINED,
    PACKET_USER_LEFT,
    
    // Server Notices
    PACKET_NOTICE,
    PACKET_WARNING,
    
    // Rooms / Channels / GROUPS
    PACKET_CREATE_ROOM,
    PACKET_DELETE_ROOM,
    PACKET_JOIN_ROOM,
    PACKET_LEAVE_ROOM,
    PACKET_ROOM_LIST,
    PACKET_ROOM_INFO,
    
    // Administration
    PACKET_KICK,
    PACKET_BAN,
    PACKET_UNBAN,
    PACKET_MUTE,
    PACKET_UNMUTE,
    PACKET_SET_ADMIN,
    PACKET_REMOVE_ADMIN,

    // File
    PACKET_FILE,

    // VOICE CHAT
    PACKET_VOICE_JOIN,
    PACKET_VOICE_LEAVE,
    PACKET_VOICE_MUTE,
    PACKET_VOICE_UNMUTE,

    // FRIEND SYSTEM
    PACKET_FRIEND_REQUEST,
    PACKET_FRIEND_ACCEPT,
    PACKET_FRIEND_REJECT,
    PACKET_REMOVE_FRIEND 
} packetType;

struct response{
    char sender[50];
    char message[1024];
};

typedef struct {
    uint32_t type;
    uint32_t size;
} packetHeader;

struct clientList{    
    struct client *clients;
    int size;
    int capacity;
};

struct client{
    char name[50];
    int clientFD;
};

struct acceptedConnection{
    int FD;
    struct sockaddr_in socketAddress;
    bool acceptedSuccessfuly;
    int error;
};

#endif