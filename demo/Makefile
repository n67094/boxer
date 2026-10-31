#
# Makefile for building: demo
#
# libboxer is statically linked, so no need to install the library to build and run the demo.
# When building the demo for the first time libboxer will be built from source and linked statically.
# To clean libboxer build artifacts, run `make clean` in the boxer directory, not in the demo directory itself.
#
# Command examples:
#
#   make                              Build the project
#   make DEBUG=1                      Build with debug symbols
#   make SANITIZER=1                  Build with address sanitizer
#   make DEBUG=1 SANITIZER=1          Build with debug symbols and address sanitizer
#   make clean                        Clean build artifacts (boxer build is kept)
#
# Authors: nsix
#

# Config
TARGET := demo

BUILD_DIR := build
SRCS_DIR := src

ASSETS_DIR := assets
ASSETS_ZIP := $(BUILD_DIR)/$(ASSETS_DIR).zip
ASSETS_FILES := $(shell find $(ASSETS_DIR) -type f)
ASSETS_EMBED := $(BUILD_DIR)/$(ASSETS_DIR).o
ASSETS_EMBED_INC := $(BUILD_DIR)/$(ASSETS_DIR).inc

# Boxer
BOXER_DIR := ..
BOXER_LIB := $(BOXER_DIR)/build/lib/libboxer.a
BOXER_INC := $(BUILD_DIR)/include
BOXER_BIN2C := $(BUILD_DIR)/bin2c

UNAME_S := $(shell uname -s)

# Options
DEBUG ?= 0
SANITIZER ?= 0

# Compiler
CC = gcc
CFLAGS = -std=c99 -Wall -I$(SRCS_DIR) -I$(BOXER_INC)
LDFLAGS = $(BOXER_LIB) -lSDL3 -lm

ifeq ($(UNAME_S), Darwin)
LDFLAGS += -framework Foundation -framework IOKit
endif

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

.PHONY: all clean FORCE
.DEFAULT_GOAL := all

all: $(TARGET)

# Create a zip archive of the assets directory
$(ASSETS_ZIP): $(ASSETS_FILES) | $(BUILD_DIR)
	rm -f $@
	zip -r $@ $(ASSETS_DIR)

# Create build directory
$(BUILD_DIR):
	mkdir -p $@

# Build boxer static library
$(BOXER_LIB): FORCE
	$(MAKE) -C $(BOXER_DIR) static DEBUG=$(DEBUG) SANITIZER=$(SANITIZER)

# Expose boxer headers using symlinks
$(BOXER_INC)/boxer: | $(BUILD_DIR)
	mkdir -p $(BOXER_INC)
	ln -sfn $(abspath $(BOXER_DIR)/include) $@

$(BUILD_DIR)/%.o: $(SRCS_DIR)/%.c | $(BUILD_DIR) $(BOXER_INC)/boxer
	$(CC) $(CFLAGS) -c -o $@ $<

# Build boxer's bin2c tool, used to turn files into C byte lists
$(BOXER_BIN2C): $(BOXER_DIR)/tools/bin2c.c | $(BUILD_DIR)
	$(CC) -std=c99 -Wall -O2 -o $@ $<

# Embed the asset archive in the executable as a C byte list
$(ASSETS_EMBED_INC): $(ASSETS_ZIP) $(BOXER_BIN2C)
	$(BOXER_BIN2C) $< $@

$(ASSETS_EMBED): CFLAGS += -I$(BUILD_DIR)
$(ASSETS_EMBED): $(ASSETS_EMBED_INC)

$(TARGET): $(OBJS) $(BOXER_LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
