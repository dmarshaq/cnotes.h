CC      	:= gcc
CFLAGS  	:= -Wextra -Wall -std=c99
SRC_DIR 	:= src
BUILD_DIR 	:= build

SRCS 		:= $(wildcard $(SRC_DIR)/*.c)
IFILES 		:= $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.i,$(SRCS))

META 		:= meta
TARGET 		:= main

OBJS 		:= $(patsubst $(BUILD_DIR)/%.i,$(BUILD_DIR)/%.o,$(IFILES))

.PHONY: all meta_proc clean

all: clean $(IFILES) meta_proc $(TARGET)

meta_proc: $(META)
	./$(META) $(IFILES)

$(BUILD_DIR)/%.i: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -E $< -o $@

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.i
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(META): meta.c defer.c
	$(CC) $(CFLAGS) meta.c defer.c -o $(META)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(META) $(TARGET)
