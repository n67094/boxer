# Authors: nsix
#
# Makefile for building the demo.
#
# libboxer is linked statically, so the library does not need to be installed.
#
# Command examples:
#
#   make                              Build the project
#   make DEBUG=1                      Build with debug symbols
#   make SANITIZER=1                  Build with address sanitizer
#   make DEBUG=1 SANITIZER=1          Build with debug symbols and address sanitizer
#   make clean                        Clean build artifacts (boxer build is kept)

UNAME_S := $(shell uname -s)

# Targets
TARGET := demo

# Directories
SRC_DIR    := src
TOOLS_DIR  := tools
ASSETS_DIR := assets
BUILD_DIR  := build
BOXER_DIR  := ..

# Options
DEBUG     ?= 0
SANITIZER ?= 0

# Boxer
BOXER_LIB := $(BOXER_DIR)/build/lib/libboxer.a
BOXER_INC := $(BUILD_DIR)/include

# Assets
ASSETS_FILES := $(shell find $(ASSETS_DIR) -type f)
ASSETS_ZIP   := $(BUILD_DIR)/assets.zip
ASSETS_INC   := $(BUILD_DIR)/assets.inc
BIN2C        := $(BUILD_DIR)/bin2c

# Sources
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Compiler
CC      := gcc
CFLAGS  := -std=c99 -Wall -I$(SRC_DIR) -I$(BUILD_DIR) -I$(BOXER_INC)
LDFLAGS :=
LDLIBS  := $(BOXER_LIB) -lSDL3 -lm

ifeq ($(UNAME_S), Darwin)
LDLIBS += -framework Foundation -framework IOKit
endif

ifeq ($(DEBUG), 1)
CFLAGS += -g -O0 -DDEBUG
else
CFLAGS += -O2
endif

ifeq ($(SANITIZER), 1)
CFLAGS  += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address
endif

.PHONY: all clean FORCE
.DEFAULT_GOAL := all

all: $(TARGET)

$(TARGET): $(OBJS) $(BOXER_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR) $(BOXER_INC)/boxer
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $@

# Boxer: always delegate to its own Makefile, which knows when to rebuild
$(BOXER_LIB): FORCE
	$(MAKE) -C $(BOXER_DIR) static DEBUG=$(DEBUG) SANITIZER=$(SANITIZER)

# Boxer: expose headers as <boxer/...> through a symlink
$(BOXER_INC)/boxer: | $(BUILD_DIR)
	mkdir -p $(BOXER_INC)
	ln -sfn $(abspath $(BOXER_DIR)/include) $@

# Assets: zip the assets directory and embed it as a C byte list (see src/assets.c)
$(BUILD_DIR)/assets.o: $(ASSETS_INC)

$(ASSETS_INC): $(ASSETS_ZIP) $(BIN2C)
	$(BIN2C) $< $@

$(ASSETS_ZIP): $(ASSETS_FILES) | $(BUILD_DIR)
	rm -f $@
	zip -r $@ $(ASSETS_DIR)

$(BIN2C): $(TOOLS_DIR)/bin2c.c | $(BUILD_DIR)
	$(CC) -std=c99 -Wall -O2 -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
