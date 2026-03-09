#
# Makefile example for building your game project.
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

ENGINE_DIR := ../src
ENGINE_SRC_DIR := $(ENGINE_DIR)
ENGINE_INC_DIR := $(ENGINE_DIR)

# Libraries Directories

LIB_DIR := ../lib

PHYSFS_SRC_DIR := $(LIB_DIR)/physfs
PHYSFS_INC_DIR := $(LIB_DIR)/physfs

# Options

DEBUG ?= 1
SANITIZER ?= 0

# Compiler

CC = gcc

LDFLAGS = -lSDL3 -lSDL3_ttf -lm

CFLAGS = -std=c99 -Wall

INC_FLAGS := -I$(PHYSFS_INC_DIR) -I$(ENGINE_INC_DIR) -I$(TEST_INC_DIR)

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
ENGINE_SRCS := $(wildcard $(ENGINE_SRC_DIR)/*.c)
# $(filter-out $(ENGINE_SRC_DIR)/engine.c, )
TEST_SRCS := $(wildcard $(TEST_SRC_DIR)/*.c)

ALL_SRCS = $(PHYSFS_SRCS) $(ENGINE_SRCS) $(TEST_SRCS)

# Object Files

PHYSFS_OBJS := $(patsubst $(PHYSFS_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(PHYSFS_SRCS))
ENGINE_OBJS := $(patsubst $(ENGINE_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(ENGINE_SRCS))
TEST_OBJS := $(patsubst $(TEST_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))

ALL_OBJS = $(PHYSFS_OBJS) $(ENGINE_OBJS) $(TEST_OBJS)

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

# Engine
$(BUILD_DIR)/%.o: $(ENGINE_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Game (User Code)
$(BUILD_DIR)/%.o: $(TEST_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) data.zip
