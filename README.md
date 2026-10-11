# Cetworking

A lightweight, educational TCP chat application written in C. The project implements a custom client/server networking model for local networks with support for user discovery, peer-to-peer messaging, group rooms, member management, and file transfer.

This repository is intentionally focused on demonstrating core networking concepts in C: sockets, TCP streams, packet serialization, multi-client handling, and thread-based I/O.

## Overview

Cetworking is designed as a local, single-host communication demo. It runs on `127.0.0.1:5000` and uses a simple command-line interface for both the server and connected clients.

The application is organized into three major logical layers:

- `src/server/`: accepts connections, manages client state, routes protocol messages, and maintains room membership
- `src/client/`: connects to the server, handles user commands, and processes incoming network events
- `src/shared/`: contains common data models, packet serialization helpers, group management, and client tracking logic

## What this project demonstrates

This codebase covers several core systems that are commonly used in networking and systems programming:

- TCP socket programming using POSIX APIs
- IPv4 socket address construction
- client connection acceptance and per-client worker threads
- custom binary packet protocol with typed messages
- length-prefixed string serialization and byte serialization
- client and group state management in memory
- asynchronous receive loops on both client and server
- file transfer between connected peers via a stream-oriented socket protocol
- room-based messaging patterns and admin/member coordination

## Tech stack

This project uses a compact, low-level software stack:

- Language: C
- Compiler: GCC via Makefile
- OS interface: POSIX/Linux socket APIs
- Network transport: TCP over IPv4 (`AF_INET`, `SOCK_STREAM`)
- Concurrency: pthreads
- Build system: GNU Make
- Internals: custom protocol structs and manual binary serialization

## Architecture

### 1. Server

The server starts by creating a TCP socket and binding it to a local address:

- `createTCPIpv4Socket()` creates the listening socket
- `createSocketAddress()` builds an IPv4 `sockaddr_in`
- `bind()` associates the socket with `127.0.0.1:5000`
- `listen()` enables the socket to accept incoming clients

The main server loop runs in `src/server/server.c` and delegates to `startAcceptingIncomingConnection()`, which continuously accepts new socket connections.

Each accepted client is handled asynchronously in its own thread through `receiveAndManageIncomingDataOnSaperateThread()` and `receiveDataFromClient()`. That thread blocks on `recvAll()` and then dispatches the packet by calling `manageServerProtocol()`.

This pattern is a classic event-driven server using one thread per client connection, with protocol handling centralized in `src/server/protocol/protocol.c`.

### 2. Client

The client process performs the following steps:

1. prompts for a display name
2. opens a TCP socket to the server
3. sends an introduction packet containing the name
4. starts a background receive thread
5. waits for user input for commands like `/chat`, `/create_group`, or `/refresh`

The client receive loop reads packet headers and calls `manageClientProtocol()`, which dispatches events like user list updates, group updates, incoming chat messages, or file transfers.

### 3. Shared layer

The shared directory contains the data model and reusable infrastructure used across both the client and server:

- `model.h`: packet types, socket metadata, client/group structures, and packet containers
- `serializer/`: custom binary encoding/decoding helpers for strings and raw bytes
- `recv_all/`: robust full-read helper for stream sockets
- `client_manager/`: dynamic client list operations
- `group/`: group creation and group-list operations

## Protocol design

The project uses a custom binary packet format rather than a text-based protocol.

The base packet header is defined in `src/shared/model.h`:

```c
struct packetHeader {
    packetType type;
    uint32_t payloadSize;
};
```

The `packetType` enum defines a wide range of message categories, including:

- chat and direct messaging
- client list synchronization
- room/group operations
- join and leave events
- file transfer packets

### Packet flow

A typical exchange is:

1. sender writes a `packetHeader`
2. sender writes a payload for the specific message type
3. receiver reads the header with `recvAll()`
4. receiver reads the full payload based on `payloadSize`
5. protocol handler parses the payload and performs the appropriate action

This design is simple and effective for local experimentation, and it is easy to trace in the code because packet dispatch is centralized in the protocol handlers.

## Serialization internals

The project implements its own serializer in `src/shared/serializer/serializer.c`.

Key helpers include:

- `packetWriterInIt()` initializes the write buffer
- `packetWriteBytes()` appends arbitrary byte data
- `packetWriteString()` writes a length-prefixed string as:
  - `uint32_t stringLength`
  - followed by the raw string bytes
- `packetReadString()` reads a length-prefixed string from a packet reader
- `packetReadBytes()` copies arbitrary payload bytes from the incoming stream

This is important because messages usually contain a combination of:

- integer socket descriptors
- string usernames
- group names
- descriptions
- raw file data

The custom serializer avoids depending on external libraries and keeps the protocol compact and explicit.

## Network reliability model

Because sockets are stream-based, the application cannot assume a single `recv()` or `send()` call transfers the entire packet. To address this, the repository uses a helper from `src/shared/recv_all/recv_all.h` and `recv_all.c`.

This helper ensures the application reads exactly the expected number of bytes before continuing, which is essential when message boundaries are defined using a packet header.

The pattern is used in places like:

- `receiveDataFromServer()` on the client
- `receiveDataFromClient()` on the server
- packet parsing functions in both protocol layers

## Client and server state management

### Client list

`src/shared/client_manager/client_manager.c` manages a dynamic list of connected clients.

The client structure is defined in `model.h` as:

```c
struct client {
    char *name;
    int FD;
};
```

This list is used to:

- track connected clients
- broadcast join/leave notifications
- resolve user names to socket file descriptors
- synchronize user lists to clients
- route direct messages

### Group list

`src/shared/group/group.c` implements room/group management, including:

- creating a new group
- storing group metadata
- adding/removing members
- printing the list of available groups

A group is represented as:

```c
struct group {
    char *name;
    char *description;
    struct client admin;
    struct clientList members;
};
```

This supports both room semantics and simple chat-channel behavior.

## Messaging model

The project supports multiple message patterns:

### Direct messaging

A client sends a direct message packet with the destination socket FD and payload. The server reads this packet and forwards it to the target connection.

### Group chat

Group messages are routed to all members in a room by looking up the room in the server-side `roomList` state and broadcasting the payload to each member socket.

### User lifecycle events

When a user connects, the server:

1. registers the client in the client list
2. sends a client list update to the new user
3. notifies existing clients about the new connection

When a user disconnects, the server removes them from the list and broadcasts a departure notification to all remaining participants.

## File transfer

File transfer is implemented as a packet-based stream transfer, using a custom `PACKET_FILE` payload structure.

The server and client each handle the transfer by parsing:

- destination socket FD
- filename
- file size
- raw file bytes

The file data is then streamed over the TCP connection in chunks until all bytes are delivered.

This is a practical demonstration of socket-stream file transport without using a higher-level networking library.

## Build system

The project uses a single Makefile at the repository root. The build target creates:

```bash
make
```

This produces:

- `build/server`
- `build/client`

The Makefile also includes compile flags such as:

- `-Wall`
- `-Wextra`
- `-g`
- `-O0`
- `-pthread`

This is a good default for learning and debugging C code, especially for networking and concurrency work.

## Running the application

### Start the server

```bash
./build/server
```

The server binds to the loopback interface and waits for clients.

### Start clients

```bash
./build/client
```

Run multiple client instances in separate terminals to simulate a local multi-user chat environment.

## Command set

The client uses slash commands for interaction.

### General commands

```bash
/help
/refresh
/create_group
/chat Alice
/open MyGroup
/leave_group MyGroup
/kick Bob
```

### Help topics

```bash
/help
/help chat
/help group
```

### Internal chat session behavior

- `/back` exits the current chat session
- `/send` initiates file transfer while chatting
- `/info` prints group information inside a group chat

These commands are parsed in `src/client/commands/command_manager/commandManager.c` and routed to specialized handlers in the command modules.

## Repository layout

```text
C-etworking/
├── README.md
├── makefile
├── .gitignore
├── build/
├── src/
│   ├── client/
│   │   ├── client.c
│   │   ├── commands/
│   │   │   ├── command_manager/
│   │   │   ├── help/
│   │   │   ├── internal_commands/
│   │   │   └── main_commands/
│   │   ├── network/
│   │   ├── protocol/
│   │   └── shared_list/
│   ├── server/
│   │   ├── server.c
│   │   ├── network/
│   │   ├── protocol/
│   │   └── shared_list/
│   └── shared/
│       ├── client_manager/
│       ├── group/
│       ├── recv_all/
│       ├── serializer/
│       ├── model.h
│       └── ...
└── ...
```

## Strengths of the implementation

- clear separation between server, client, and shared logic
- practical use of socket APIs and threads
- custom packet protocol instead of a framework abstraction
- explicit state management for users and rooms
- direct educational value for systems/networking coursework

## Limitations and caveats

This is a learning-oriented project rather than a production-grade messaging platform. The implementation is intentionally lightweight and has some rough edges, including:

- no authentication or authorization layer
- no encryption or TLS
- no persistent storage or database
- minimal input validation
- no production-focused error recovery or reconnection strategy
- local-only design assumptions

This is fine for demonstrating networking fundamentals, but it should not be treated as a secure or production-ready system.

## Use cases

This repository is well suited for:

- learning socket programming in C
- exploring message protocol design
- studying TCP client/server interaction
- experimenting with thread-per-client architectures
- building small systems projects with custom binary protocols

## Summary

Cetworking is a compact, educational C networking project that demonstrates how to build a TCP chat application from the socket layer up. It covers local network communication, custom message framing, concurrent client handling, room-based communication, and file transfer over stream sockets.

It is most valuable as a technical learning project for engineers and students interested in systems programming, networking, protocol design, and real-world C application architecture.

## License

This repository does not currently declare a license in the GitHub metadata.
