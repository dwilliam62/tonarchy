CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -O2 -Iinclude
LDFLAGS =

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
TARGET = tonarchy

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean install

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) /usr/local/bin/$(TARGET)
	cp -r packages /usr/share/tonarchy/
	cp -r configs /usr/share/tonarchy/
