CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -Iinclude -D_GNU_SOURCE
LDFLAGS = -lpthread

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SERVER_SRC = $(SRC_DIR)/server.c $(SRC_DIR)/file_ops.c
SERVER_EPOLL_SRC = $(SRC_DIR)/server_epoll.c $(SRC_DIR)/file_ops.c
CLIENT_SRC = $(SRC_DIR)/client.c $(SRC_DIR)/file_ops.c

SERVER_OBJ = $(SERVER_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
SERVER_EPOLL_OBJ = $(SERVER_EPOLL_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CLIENT_OBJ = $(CLIENT_SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

SERVER_BIN = $(BIN_DIR)/bank_server
SERVER_EPOLL_BIN = $(BIN_DIR)/bank_server_epoll
CLIENT_BIN = $(BIN_DIR)/bank_client

.PHONY: all clean test run-server run-server-epoll run-client debug server_epoll

all: $(SERVER_BIN) $(CLIENT_BIN) $(SERVER_EPOLL_BIN)

$(SERVER_BIN): $(SERVER_OBJ) | $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

server_epoll: $(SERVER_EPOLL_OBJ) | $(BIN_DIR)
	$(CC) -o $(SERVER_EPOLL_BIN) $^ $(LDFLAGS)

$(SERVER_EPOLL_BIN): $(SERVER_EPOLL_OBJ) | $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(CLIENT_BIN): $(CLIENT_OBJ) | $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c include/bank.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) data/*.dat

run-server: $(SERVER_BIN)
	./$(SERVER_BIN)

run-server-epoll: $(SERVER_EPOLL_BIN)
	./$(SERVER_EPOLL_BIN)

run-client: $(CLIENT_BIN)
	./$(CLIENT_BIN)

debug: CFLAGS += -DDEBUG -fsanitize=address -fno-omit-frame-pointer
debug: LDFLAGS += -fsanitize=address
debug: all

test: $(SERVER_BIN) $(CLIENT_BIN)
	@echo "Running basic tests..."
	@./tests/test_basic.sh

install: all
	cp $(SERVER_BIN) /usr/local/bin/
	cp $(CLIENT_BIN) /usr/local/bin/

help:
	@echo "Targets:"
	@echo "  all         - Build server and client"
	@echo "  clean       - Remove build artifacts and data files"
	@echo "  run-server  - Run the bank server"
	@echo "  run-client  - Run the bank client"
	@echo "  debug       - Build with address sanitizer"
	@echo "  test        - Run basic tests"
	@echo "  install     - Install to /usr/local/bin"