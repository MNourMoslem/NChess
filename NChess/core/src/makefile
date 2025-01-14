CC = gcc
CFLAGS = -Wall -Wextra -std=c11
SRC_DIR = .
OBJ_DIR = $(SRC_DIR)/obj
BIN_DIR = $(SRC_DIR)/bin
TARGET = $(BIN_DIR)/nchess

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(OBJS)
	@mkdir -p $(BIN_DIR)
	@ar rcs $(BIN_DIR)/libnchess.a $(OBJS)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^
	@ar rcs $(BIN_DIR)/libnchess.a $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run