#
# Command examples:
#
#   make                       Build the project (debug is enabled by default)
#   make DEBUG=0               Build without debug symbols
#   make SANITIZER=1           Build with address sanitizer
#   make clean                 Clean build artifacts
#
# Authors: nsix
#

TARGET := test

BUILD_DIR := build

DATA_DIR := data

TEST_SRC_DIR := src
TEST_INC_DIR := src

BOXER_DIR := ../src
BOXER_SRC_DIR := $(BOXER_DIR)
BOXER_INC_DIR := $(BOXER_DIR)

# Libraries Directories

LIB_DIR := ../lib

PHYSFS_SRC_DIR := $(LIB_DIR)/physfs
PHYSFS_INC_DIR := $(LIB_DIR)/physfs

SDL_GP_INC_DIR := $(LIB_DIR)/SDL_gp

# Options

DEBUG ?= 1
SANITIZER ?= 0

# Compiler

CC = gcc

LDFLAGS = -lSDL3 -lm

CFLAGS = -std=c99 -Wall

INC_FLAGS := -I$(PHYSFS_INC_DIR) -I$(SDL_GP_INC_DIR) -I$(BOXER_INC_DIR) -I$(TEST_INC_DIR)

ifeq ($(DEBUG), 1)
CFLAGS += -g -O0 -DDEBUG -D_GNU_SOURCE  $(INC_FLAGS)
else
CFLAGS += -O2 -D_GNU_SOURCE $(INC_FLAGS)
endif

ifeq ($(SANITIZER), 1)
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address
endif

# Source Files

PHYSFS_SRCS := $(wildcard $(PHYSFS_SRC_DIR)/*.c)
BOXER_SRCS := $(wildcard $(BOXER_SRC_DIR)/*.c)
TEST_SRCS := $(wildcard $(TEST_SRC_DIR)/*.c)

ALL_SRCS = $(PHYSFS_SRCS) $(BOXER_SRCS) $(TEST_SRCS)

# Object Files

PHYSFS_OBJS := $(patsubst $(PHYSFS_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(PHYSFS_SRCS))
BOXER_OBJS := $(patsubst $(BOXER_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(BOXER_SRCS))
TEST_OBJS := $(patsubst $(TEST_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))

ALL_OBJS = $(PHYSFS_OBJS) $(BOXER_OBJS) $(TEST_OBJS)

# Targets

.PHONY: all data clean
.DEFAULT_GOAL := all

all: $(TARGET) data

data:
	zip -r data.zip $(DATA_DIR)

$(TARGET): $(ALL_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

# PhysFS
$(BUILD_DIR)/%.o: $(PHYSFS_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Boxer
$(BUILD_DIR)/%.o: $(BOXER_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Game (User Code)
$(BUILD_DIR)/%.o: $(TEST_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) data.zip
