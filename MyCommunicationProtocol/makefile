CC := gcc
BUILD_DIR := build
CFLAGS := -Wall -Wextra -pthread -I./src -I./src/client -I./src/client/Network -I./src/client/protocol -I./src/client/serializer -I./src/server -I./src/server/network -I./src/server/protocol -I./src/server/client\ manager

CLIENT_SOURCES := \
	src/client/client.c \
	src/client/Network/network.c \
	src/client/protocol/protocol.c \
	src/client/serializer/serializer.c

SERVER_SOURCES := \
	src/server/server.c \
	src/server/network/network.c \
	src/server/protocol/protocol.c \
	src/client/serializer/serializer.c \
	src/server/client\ manager/client_manager.c

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
