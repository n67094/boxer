#
# Makefile for building: demo
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
ASSETS_ZIP := $(ASSETS_DIR).zip

# Boxer (built from source, linked statically)
BOXER_DIR := ..
BOXER_LIB := $(BOXER_DIR)/build/lib/libboxer.a
BOXER_INC := $(BUILD_DIR)/include

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

all: $(TARGET) $(ASSETS_ZIP)

# Create a zip archive of the assets directory
$(ASSETS_ZIP): $(ASSETS_DIR)
	zip -r $@ $<

# Create build directory
$(BUILD_DIR):
	mkdir -p $@

# Build boxer static library (always delegated, boxer's Makefile decides what to rebuild)
$(BOXER_LIB): FORCE
	$(MAKE) -C $(BOXER_DIR) static DEBUG=$(DEBUG) SANITIZER=$(SANITIZER)

# Expose boxer headers as <boxer/...> without installing them
$(BOXER_INC)/boxer: | $(BUILD_DIR)
	mkdir -p $(BOXER_INC)
	ln -sfn $(abspath $(BOXER_DIR)/include) $@

$(BUILD_DIR)/%.o: $(SRCS_DIR)/%.c | $(BUILD_DIR) $(BOXER_INC)/boxer
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS) $(BOXER_LIB)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(ASSETS_ZIP) $(TARGET)
