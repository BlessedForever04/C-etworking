# Cetworking

A lightweight TCP chat application written in C for local network experiments. The project follows a client/server model and supports peer-to-peer chat, group rooms, member management, and file transfer.

## Overview

This repository contains:

- a C server that accepts incoming client connections and routes messages
- a C client that lets users connect, chat, manage rooms, and send files
- shared networking utilities for packet serialization and socket communication
- room/group logic for chat channels and membership tracking

The application is designed for local testing on `127.0.0.1:5000` and is built using a simple Makefile.

## Architecture

- `src/server/` - server-side networking and protocol handling
- `src/client/` - client application, command parsing, and network code
- `src/shared/` - common data structures, client/group management, packet serialization, and socket helpers
- `makefile` - builds both the server and client binaries
- `build/` - compiled outputs created by the build process

## Features

- User login with a display name
- Client list synchronization from the server
- Direct one-to-one messaging
- Group creation and group membership management
- Group chat inside a selected room
- Admin-based member removal and room departure
- File transfer between connected clients
- Basic command-driven CLI interface

## Build

From the project root:

```bash
make
```

This creates:

```bash
./build/server
./build/client
```

## Run

Start the server in one terminal:

```bash
./build/server
```

Then start clients in separate terminals:

```bash
./build/client
```

When prompted, enter a username for each client.

## Client Commands

The client accepts commands prefixed with `/`.

Common examples:

```bash
/help
/refresh
/create_group
/chat Alice
/open MyGroup
/leave_group MyGroup
/kick Bob
```

Inside an active chat session:

```bash
/back
/send
```

- `/back` exits the current chat context
- `/send` triggers a file transfer while chatting with a user
- `/info` prints group information while inside a group

## Protocol and networking model

The project uses custom packet headers and a lightweight serializer to transmit structured messages over TCP. Shared types are defined in `src/shared/model.h`, and serialization helpers are in `src/shared/serializer/serializer.c`.

The server listens for incoming connections, parses packet types, and forwards messages to the correct destination client or room members. The client receives packets asynchronously on a background thread and dispatches them to handlers based on packet type.

## Notes

- The project is intended for local development and learning, not as a production-grade chat platform.
- The default port is `5000` and the host is `127.0.0.1`.
- There are still rough edges in some command paths and protocol handling, but the project demonstrates a practical C networking/chat architecture.

## Project Structure

```text
C-etworking/
├── .gitignore
├── README.md
├── makefile
├── build/
├── src/
│   ├── client/
│   │   ├── commands/
│   │   ├── network/
│   │   ├── protocol/
│   │   ├── shared_list/
│   │   └── client.c
│   ├── server/
│   │   ├── network/
│   │   ├── protocol/
│   │   ├── shared_list/
│   │   └── server.c
│   └── shared/
│       ├── client_manager/
│       ├── group/
│       ├── recv_all/
│       ├── serializer/
│       ├── model.h
│       └── ...
└── ...
```

## License

This project does not currently declare a license in the repository metadata.

## Summary

Cetworking is a small educational C networking project that demonstrates socket programming, custom packet protocols, client/server communication, and a basic chat application with group features.


