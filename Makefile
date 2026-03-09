#
# Makefile example for building your game project.
#
# Please read through it to understand:
#
# - The project structure,
# - how compilation is done,
# - How to add libraries,
# - How to enable debug symbols and sanitizers.
# - Etc.
#
# Command examples:
#
#   make                       Build the project
#   make DEBUG=1               Build with debug symbols
#   make SANITIZER=1           Build with address sanitizer
#   make DEBUG=1 SANITIZER=1   Build with debug symbols and address sanitizer
#   make clean                 Clean build artifacts
#
# Authors: nsix
#

TARGET := my_game

# Directories

BUILD_DIR := build

DATA_DIR := data

GAME_DIR := game
GAME_SRC_DIR := $(GAME_DIR)
GAME_INC_DIR := $(GAME_DIR)

BOXER_DIR := src
BOXER_SRC_DIR := $(BOXER_DIR)
BOXER_INC_DIR := $(BOXER_DIR)

# Libraries Directories

LIB_DIR := lib

PHYSFS_SRC_DIR := $(LIB_DIR)/physfs
PHYSFS_INC_DIR := $(LIB_DIR)/physfs

# Options

DEBUG ?= 0
SANITIZER ?= 0

# Compiler

CC = gcc
CXX = g++

LDFLAGS = -lSDL3 -lSDL3_ttf -lm

CFLAGS = -std=c99 -Wall
CXXFLAGS = -std=c++11 -Wall

INC_FLAGS := -I$(PHYSFS_INC_DIR) -I$(BOXER_INC_DIR) -I$(GAME_INC_DIR)

ifeq ($(DEBUG), 1)
CFLAGS += -g -O0 -DDEBUG -D_GNU_SOURCE $(INC_FLAGS)
else
CFLAGS += -O2 -D_GNU_SOURCE $(INC_FLAGS)
endif

ifeq ($(SANITIZER), 1)
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address
endif

# Source Files

PHYSFS_SRCS := $(wildcard $(PHYSFS_SRC_DIR)/*.c)
BOXER_SRCS := $(wildcard $(BOXER_SRC_DIR)/*.c)
GAME_SRCS := $(wildcard $(GAME_SRC_DIR)/*.c)

ALL_SRCS = $(PHYSFS_SRCS) $(BOXER_SRCS) $(GAME_SRCS)

# Object Files

PHYSFS_OBJS := $(patsubst $(PHYSFS_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(PHYSFS_SRCS))
BOXER_OBJS := $(patsubst $(BOXER_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(BOXER_SRCS))
GAME_OBJS := $(patsubst $(GAME_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(GAME_SRCS))

ALL_OBJS = $(PHYSFS_OBJS) $(BOXER_OBJS) $(GAME_OBJS)

# Targets

.PHONY: all data docs clean
.DEFAULT_GOAL := all

all: $(TARGET) data

data:
	zip -r data.zip $(DATA_DIR)

docs:
	doxygen

$(TARGET): $(ALL_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

# PhysFS
$(BUILD_DIR)/%.o: $(PHYSFS_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Boxer
$(BUILD_DIR)/%.o: $(BOXER_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Game (User Code)
$(BUILD_DIR)/%.o: $(GAME_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) data.zip
