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

ENGINE_DIR := engine
ENGINE_SRC_DIR := $(ENGINE_DIR)
ENGINE_INC_DIR := $(ENGINE_DIR)

# DEBUG_DIR := debug
# DEBUG_SRC_DIR := $(DEBUG_DIR)
# DEBUG_INC_DIR := $(DEBUG_DIR)

# Libraries Directories

LIB_DIR := lib

# IMGUI_SRC_DIR := $(LIB_DIR)/dcimgui
# IMGUI_INC_DIR := $(LIB_DIR)/dcimgui

PHYSFS_SRC_DIR := $(LIB_DIR)/physfs
PHYSFS_INC_DIR := $(LIB_DIR)/physfs

# Options

DEBUG ?= 0
SANITIZER ?= 0

# Compiler

CC = gcc
CXX = g++

# LDFLAGS = -lSDL3 -lSDL_mixer -lSDL_ttf -lphysfs -lm
LDFLAGS = -lSDL3 -lSDL_ttf -lphysfs -lm

CFLAGS = -std=c99 -Wall
CXXFLAGS = -std=c++11 -Wall

INC_FLAGS := -I$(PHYSFS_INC_DIR) -I$(ENGINE_INC_DIR) -I$(GAME_INC_DIR)

ifeq ($(DEBUG), 1)
# CFLAGS += -g -O0 -DDEBUG -D_GNU_SOURCE -I$(IMGUI_INC_DIR) -I$(DEBUG_INC_DIR) $(INC_FLAGS)
CFLAGS += -g -O0 -DDEBUG -D_GNU_SOURCE $(INC_FLAGS)
# CXXFLAGS += -g -O0 -DDEBUG
else
CFLAGS += -O2 -D_GNU_SOURCE $(INC_FLAGS)
# CXXFLAGS += -O2
endif

ifeq ($(SANITIZER), 1)
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
# CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address
endif

# Source Files

PHYSFS_SRCS := $(wildcard $(PHYSFS_SRC_DIR)/*.c)
ENGINE_SRCS := $(wildcard $(ENGINE_SRC_DIR)/*.c)
GAME_SRCS := $(wildcard $(GAME_SRC_DIR)/*.c)

ALL_SRCS = $(PHYSFS_SRCS) $(ENGINE_SRCS) $(GAME_SRCS)

# ifeq ($(DEBUG), 1)
# IMGUI_SRCS := $(wildcard $(IMGUI_SRC_DIR)/*.cpp)
# DEBUG_SRCS := $(wildcard $(DEBUG_SRC_DIR)/*.c)

# ALL_SRCS += $(IMGUI_SRCS) $(DEBUG_SRCS)
# endif

# Object Files

PHYSFS_OBJS := $(patsubst $(PHYSFS_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(PHYSFS_SRCS))
ENGINE_OBJS := $(patsubst $(ENGINE_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(ENGINE_SRCS))
GAME_OBJS := $(patsubst $(GAME_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(GAME_SRCS))

ALL_OBJS = $(PHYSFS_OBJS) $(ENGINE_OBJS) $(GAME_OBJS)

# ifeq ($(DEBUG), 1)
# IMGUI_OBJS := $(patsubst $(IMGUI_SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(IMGUI_SRCS))
# DEBUG_OBJS := $(patsubst $(DEBUG_SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(DEBUG_SRCS))

# ALL_OBJS += $(IMGUI_OBJS) $(DEBUG_OBJS)
# endif

# Targets

.PHONY: all data docs clean
.DEFAULT_GOAL := all

all: $(TARGET) data

data:
	zip -r data.zip $(DATA_DIR)

# docs:
# doxygen

$(TARGET): $(ALL_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

# PhysFS
$(BUILD_DIR)/%.o: $(PHYSFS_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Dear ImGui (Debug only)
# $(BUILD_DIR)/%.o: $(IMGUI_SRC_DIR)/%.cpp | $(BUILD_DIR)
# $(CXX) $(CXXFLAGS) -c $< -o $@

# Engine
$(BUILD_DIR)/%.o: $(ENGINE_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Debug Code (Debug only)
# $(BUILD_DIR)/%.o: $(DEBUG_SRC_DIR)/%.c | $(BUILD_DIR)
#	$(CC) $(CFLAGS) -c $< -o $@

# Game (User Code)
$(BUILD_DIR)/%.o: $(GAME_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET) data.zip
