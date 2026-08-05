CC := gcc
BUILD_DIR := build
CFLAGS := \
	-g \
	-O0 \
	-Wall \
	-Wextra \
	-pthread \
	-I./src \
	-I./src/shared \
	-I./src/client \
	-I./src/client/commands \
	-I./src/client/network \
	-I./src/client/protocol \
	-I./src/client/serializer \
	-I./src/server \
	-I./src/server/network \
	-I./src/server/protocol \
	-I./src/server/client_manager

CLIENT_SOURCES := \
	src/shared/recv_all.c \
	src/shared/group.c \
	src/client/client.c \
	src/client/network/network.c \
	src/client/protocol/protocol.c \
	src/client/serializer/serializer.c \
	src/client/commands/mainCommands.c \
	src/client/commands/commands.c

SERVER_SOURCES := \
	src/shared/recv_all.c \
	src/server/server.c \
	src/server/network/serverNetwork.c \
	src/server/protocol/protocol.c \
	src/client/serializer/serializer.c \
	src/server/client_manager/client_manager.c

.PHONY: all clean client server

all: client server

client: $(BUILD_DIR)/client

server: $(BUILD_DIR)/server

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/client: $(BUILD_DIR) $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) $(CLIENT_SOURCES) -o $@

$(BUILD_DIR)/server: $(BUILD_DIR) $(SERVER_SOURCES)
	$(CC) $(CFLAGS) $(SERVER_SOURCES) -o $@

clean:
	rm -f $(BUILD_DIR)/client $(BUILD_DIR)/server