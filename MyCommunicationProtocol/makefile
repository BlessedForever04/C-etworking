CC := gcc
BUILD_DIR := build
CFLAGS := \
	-g \
	-O0 \
	-Wall \
	-Wextra \
	-pthread \
	-I./src \
	-I./src/client \
	-I./src/client/commands \
	-I./src/client/network \
	-I./src/client/protocol \
	-I./src/client/shared_list \
	-I./src/server \
	-I./src/server/client_manager \
	-I./src/server/network \
	-I./src/server/protocol \
	-I./src/server/shared_list \
	-I./src/shared

CLIENT_SOURCES := \
	src/client/commands/command_manager/commandManager.c \
	src/client/commands/internal_commands/internalCommands.c \
	src/client/commands/main_commands/mainCommands.c \
	src/client/network/network.c \
	src/client/protocol/protocol.c \
	src/client/shared_list/shared_list.c \
	src/client/client.c \
	src/shared/group/group.c \
	src/shared/recv_all/recv_all.c \
	src/shared/serializer/serializer.c 

SERVER_SOURCES := \
	src/server/client_manager/client_manager.c \
	src/server/network/serverNetwork.c \
	src/server/protocol/protocol.c \
	src/server/shared_list/shared_list.c \
	src/server/server.c \
	src/shared/group/group.c \
	src/shared/recv_all/recv_all.c \
	src/shared/serializer/serializer.c \

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