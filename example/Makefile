#
# Makefile for building: example
#
# Command examples:
#
#   make                              Build the project
#   make DEBUG=1                      Build with debug symbols
#   make SANITIZER=1                  Build with address sanitizer
#   make DEBUG=1 SANITIZER=1          Build with debug symbols and address sanitizer
#   make clean                        Clean build artifacts
#
# Authors: nsix
#

# Config
TARGET := example

BUILD_DIR := build
SRCS_DIR := src

ASSETS_DIR := assets
ASSETS_ZIP := $(ASSETS_DIR).zip

# Options
DEBUG ?= 0
SANITIZER ?= 0

# Compiler
CC = gcc
CFLAGS = -std=c99 -Wall -I$(SRCS_DIR)
LDFLAGS = -lboxer -lSDL3

# Compile with debug
ifeq ($(DEBUG), 1)
CFLAGS += -g -O0 -DDEBUG
else
CFLAGS += -O2
endif

# Conpile with sanitizer
ifeq ($(SANITIZER), 1)
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address
endif

# Source files
SRCS := $(wildcard $(SRCS_DIR)/*.c)
OBJS := $(patsubst $(SRCS_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

.PHONY: all clean
.DEFAULT_GOAL := all

all: $(TARGET) $(ASSETS_ZIP)

# Create a zip archive of the assets directory
$(ASSETS_ZIP): $(ASSETS_DIR)
	zip -r $@ $<

# Create build directory
$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: $(SRCS_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(ASSETS_ZIP) $(TARGET)
