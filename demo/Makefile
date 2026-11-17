# Authors: nsix
#
# Makefile for building the demo.
#
# `make` produces demo (Linux/macOS) or demo.exe (Windows): a single executable
# with libboxer, SDL3 and the assets embedded.
#
# This requires a static SDL3 (libSDL3.a) in the linker search path:
#   Linux:   distribution package or SDL3 built with -DSDL_STATIC=ON
#   macOS:   SDL3 built with -DSDL_STATIC=ON, by default /usr/local/lib/libSDL3.a
#            (or SDL3_LIB=/path/to/libSDL3.a)
#   Windows: MSYS2, from its MinGW64 shell:
#            pacman -S make zip mingw-w64-x86_64-gcc mingw-w64-x86_64-sdl3
#
# Command examples:
#
#   make                              Build the project
#   make DEBUG=1                      Build with debug symbols
#   make SANITIZER=1                  Build with address sanitizer
#   make DEBUG=1 SANITIZER=1          Build with debug symbols and address sanitizer
#   make clean                        Clean build artifacts (boxer build is kept)
#
# Windows (from an MSYS2 MinGW shell):
#
#   make RELEASE=1                    Build without a console window

# Options
DEBUG     ?= 0
SANITIZER ?= 0
RELEASE   ?= 0

# Platform: everything needed to link a standalone executable.
# SDL3 is linked statically, followed by the system libraries it (and boxer) needs.
# GNU ld (Linux, MinGW) is told to prefer libSDL3.a over the shared library.
ifeq ($(OS), Windows_NT)
EXE              := .exe
PLATFORM_CFLAGS  := -DBXR_STATIC
PLATFORM_LDFLAGS := -static-libgcc $(if $(filter 1,$(RELEASE)),-mwindows)
PLATFORM_LIBS    := -Wl,-Bstatic -lSDL3 -Wl,-Bdynamic \
                    -lkernel32 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 \
                    -lversion -luuid -ladvapi32 -lsetupapi -lshell32 -ldinput8 -luserenv
else ifeq ($(shell uname -s), Darwin)
EXE              :=
SDL3_LIB         ?= /usr/local/lib/libSDL3.a
PLATFORM_LIBS    := $(SDL3_LIB) \
                    -framework CoreMedia -framework CoreVideo -framework Cocoa \
                    -framework IOKit -framework ForceFeedback -framework Carbon \
                    -framework CoreAudio -framework AudioToolbox \
                    -framework AVFoundation -framework Foundation -framework Metal \
                    -weak_framework UniformTypeIdentifiers -weak_framework GameController \
                    -weak_framework QuartzCore -weak_framework CoreHaptics
else
EXE              :=
PLATFORM_LIBS    := -Wl,-Bstatic -lSDL3 -Wl,-Bdynamic -ldl -pthread
endif

# Targets
TARGET := demo$(EXE)

# Directories
SRC_DIR    := src
TOOLS_DIR  := tools
ASSETS_DIR := assets
BUILD_DIR  := build
BOXER_DIR  := ..

# Boxer
BOXER_LIB := $(BOXER_DIR)/build/lib/libboxer.a
BOXER_INC := $(BUILD_DIR)/include

# Assets
ASSETS_FILES := $(shell find $(ASSETS_DIR) -type f)
ASSETS_ZIP   := $(BUILD_DIR)/assets.zip
ASSETS_INC   := $(BUILD_DIR)/assets.inc
BIN2C        := $(BUILD_DIR)/bin2c$(EXE)

# Sources
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Compiler
CC      := gcc
CFLAGS  := -std=c99 -Wall -I$(SRC_DIR) -I$(BUILD_DIR) -I$(BOXER_INC) $(PLATFORM_CFLAGS)
LDFLAGS := $(PLATFORM_LDFLAGS)
LDLIBS  := $(BOXER_LIB) $(PLATFORM_LIBS) -lm

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
