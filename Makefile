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
#   make docs                  Generate documentation (using docgen)
#   make clean                 Clean build artifacts
#
# Authors: nsix
#

TARGET := my_game

# Directories

BUILD_DIR := build
DATA_DIR := data
GAME_DIR := game
BOXER_DIR := src
BOXER_DOCS_DIR := docs

# Libraries Directories

LIB_DIR := lib
PHYSFS_DIR := $(LIB_DIR)/physfs
SDL_GP_DIR := $(LIB_DIR)/SDL_gp

# Options

DEBUG ?= 0
SANITIZER ?= 0

# Compiler

CC = gcc
CXX = g++

LDFLAGS = -lSDL3 -lm

CFLAGS = -std=c99 -Wall
CXXFLAGS = -std=c++11 -Wall

INC_FLAGS := -I$(PHYSFS_DIR) -I$(SDL_GP_DIR) -I$(BOXER_DIR) -I$(GAME_DIR)

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

PHYSFS_SRCS := $(wildcard $(PHYSFS_DIR)/*.c)
BOXER_SRCS := $(wildcard $(BOXER_DIR)/*.c)
GAME_SRCS := $(wildcard $(GAME_DIR)/*.c)

ALL_SRCS = $(PHYSFS_SRCS) $(BOXER_SRCS) $(GAME_SRCS)

# Object Files

PHYSFS_OBJS := $(patsubst $(PHYSFS_DIR)/%.c, $(BUILD_DIR)/%.o, $(PHYSFS_SRCS))
BOXER_OBJS := $(patsubst $(BOXER_DIR)/%.c, $(BUILD_DIR)/%.o, $(BOXER_SRCS))
GAME_OBJS := $(patsubst $(GAME_DIR)/%.c, $(BUILD_DIR)/%.o, $(GAME_SRCS))

ALL_OBJS = $(PHYSFS_OBJS) $(BOXER_OBJS) $(GAME_OBJS)

# Docs files

BOXER_HDRS := $(wildcard $(BOXER_DIR)/*.h)

BOXER_DOCS := $(patsubst $(BOXER_DIR)/%.h, $(BOXER_DOCS_DIR)/%.md, $(BOXER_HDRS))

# Targets

.PHONY: all data docs clean
.DEFAULT_GOAL := all

all: $(TARGET) data

# Package data directory into a zip file
data:
	zip -r data.zip $(DATA_DIR)

# Use docgen to generate documentation for Boxer headers
docs: $(BOXER_DOCS)

$(BOXER_DOCS_DIR)/%.md: $(BOXER_DIR)/%.h
	node $(BOXER_DOCS_DIR)/docgen/docgen.js $< -o $@

# Build the executable
$(TARGET): $(ALL_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

# PhysFS
$(BUILD_DIR)/%.o: $(PHYSFS_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Boxer
$(BUILD_DIR)/%.o: $(BOXER_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Game (User Code)
$(BUILD_DIR)/%.o: $(GAME_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET) data.zip
