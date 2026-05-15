#
# Makefile example for building your game project.
#
# Command examples:
#
#   make                              Build the project
#   make DEBUG=1                      Build with debug symbols
#   make SANITIZER=1                  Build with address sanitizer
#   make DEBUG=1 SANITIZER=1          Build with debug symbols and address sanitizer
#   make install PREFIX=/usr/local    Install the executable to /usr/local
#   make uninstall                    Uninstall library, executables, headers, and docs
#   make docs                         Generate documentation (using docgen)
#   make test                         Build and run tests
#   make clean                        Clean build artifacts
#
# Authors: nsix
#

TARGET := boxer
TEST_TARGET := tests

# Versioning (used for SONAME on the shared lib)

VERSION       := 0.0.0
SOVERSION     := 0

# Directories

BOXER_DOCS_DIR := docs
BOXER_SRC_DIR := src
BOXER_INC_DIR := include

BUILD_DIR := build
OBJ_DIR       := $(BUILD_DIR)/obj
PIC_OBJ_DIR   := $(BUILD_DIR)/obj_pic
LIB_OUT_DIR   := $(BUILD_DIR)/lib
TEST_BIN_DIR := $(BUILD_DIR)/test
DOCS_OUT_DIR := $(BUILD_DIR)/docs

TEST_DIR := test

# Libraries Directories

LIB_DIR := lib

PHYSFS_DIR := $(LIB_DIR)/physfs
SDL_GP_DIR := $(LIB_DIR)/SDL_gp

# Options

DEBUG ?= 0
SANITIZER ?= 0
PREFIX  ?= /usr/local

# Output library names

STATIC_LIB := $(LIB_OUT_DIR)/lib$(TARGET).a
SHARED_LIB := $(LIB_OUT_DIR)/lib$(TARGET).so.$(VERSION)
SHARED_SO  := $(LIB_OUT_DIR)/lib$(TARGET).so.$(SOVERSION)
SHARED_LNK := $(LIB_OUT_DIR)/lib$(TARGET).so

# Compiler

CC = gcc
AR = ar

# Flags
LDLIBS  = -lSDL3 -lm
LDFLAGS =

CFLAGS = -std=c99 -Wall -I$(PHYSFS_DIR) -I$(SDL_GP_DIR) -I$(BOXER_INC_DIR)

ifeq ($(DEBUG), 1)
CFLAGS += -g -O0 -DDEBUG
else
CFLAGS += -O2
endif

CFLAGS += -D_GNU_SOURCE

ifeq ($(SANITIZER), 1)
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address
endif

# Hide internal symbols by default (good practice for shared libs)
CFLAGS += -fvisibility=hidden

# Test compilation flags

TEST_CFLAGS  := $(CFLAGS) -I$(TEST_DIR)
TEST_LDFLAGS := $(LDFLAGS)
TEST_LDLIBS  := $(STATIC_LIB) $(LDLIBS)

# Source Files

PHYSFS_SRCS := $(wildcard $(PHYSFS_DIR)/*.c)
BOXER_SRCS := $(wildcard $(BOXER_SRC_DIR)/*.c)
ALL_SRCS = $(PHYSFS_SRCS) $(BOXER_SRCS)

# Object Files

PHYSFS_OBJS := $(patsubst $(PHYSFS_DIR)/%.c, $(OBJ_DIR)/physfs/%.o, $(PHYSFS_SRCS))
BOXER_OBJS := $(patsubst $(BOXER_SRC_DIR)/%.c, $(OBJ_DIR)/boxer/%.o, $(BOXER_SRCS))
ALL_OBJS = $(PHYSFS_OBJS) $(BOXER_OBJS)

# Object Files (PIC build for the shared library)

PHYSFS_PIC_OBJS := $(patsubst $(PHYSFS_DIR)/%.c,    $(PIC_OBJ_DIR)/physfs/%.o, $(PHYSFS_SRCS))
BOXER_PIC_OBJS  := $(patsubst $(BOXER_SRC_DIR)/%.c, $(PIC_OBJ_DIR)/boxer/%.o,  $(BOXER_SRCS))
ALL_PIC_OBJS    := $(PHYSFS_PIC_OBJS) $(BOXER_PIC_OBJS)

# Docs files

BOXER_HDRS := $(wildcard $(BOXER_INC_DIR)/*.h)
BOXER_DOCS := $(patsubst $(BOXER_INC_DIR)/%.h, $(DOCS_OUT_DIR)/%.md, $(BOXER_HDRS))

# Tests files

TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/tests/%.o, $(TEST_SRCS))

# Targets

.PHONY: all static shared test docs clean install uninstall
.DEFAULT_GOAL := all

all: static shared

static: $(STATIC_LIB)
shared: $(SHARED_LNK)

# --- Static library ---------------------------------------------------------

$(STATIC_LIB): $(ALL_OBJS) | $(LIB_OUT_DIR)
	$(AR) rcs $@ $^

# --- Shared library ---------------------------------------------------------

$(SHARED_LIB): $(ALL_PIC_OBJS) | $(LIB_OUT_DIR)
	$(CC) -shared -Wl,-soname,lib$(TARGET).so.$(SOVERSION) \
	$(LDFLAGS) -o $@ $^ $(LDLIBS)

# Versioned symlinks: libboxer.so -> libboxer.so.1 -> libboxer.so.1.0.0
$(SHARED_SO): $(SHARED_LIB)
	ln -sf $(notdir $<) $@

$(SHARED_LNK): $(SHARED_SO)
	ln -sf $(notdir $<) $@

# --- Compile rules ----------------------------------------------------------

# Static (non-PIC) objects
$(OBJ_DIR)/physfs/%.o: $(PHYSFS_DIR)/%.c | $(OBJ_DIR)/physfs
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/boxer/%.o: $(BOXER_SRC_DIR)/%.c | $(OBJ_DIR)/boxer
	$(CC) $(CFLAGS) -DBOXER_BUILD -c $< -o $@

# PIC objects (for the shared library)
$(PIC_OBJ_DIR)/physfs/%.o: $(PHYSFS_DIR)/%.c | $(PIC_OBJ_DIR)/physfs
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(PIC_OBJ_DIR)/boxer/%.o: $(BOXER_SRC_DIR)/%.c | $(PIC_OBJ_DIR)/boxer
	$(CC) $(CFLAGS) -fPIC -DBOXER_BUILD -c $< -o $@

# --- Directories ------------------------------------------------------------

$(OBJ_DIR)/physfs $(OBJ_DIR)/boxer $(PIC_OBJ_DIR)/physfs $(PIC_OBJ_DIR)/boxer $(OBJ_DIR)/tests  $(LIB_OUT_DIR) $(DOCS_OUT_DIR) $(TEST_BIN_DIR):
	mkdir -p $@

# --- Documentation ----------------------------------------------------------

docs: $(BOXER_DOCS)

$(DOCS_OUT_DIR)/%.md: $(BOXER_INC_DIR)/%.h | $(DOCS_OUT_DIR)
	node $(BOXER_DOCS_DIR)/docgen/docgen.js $< -o $@

# --- Test ------------------------------------------------------------------

# Build all tests
test: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(STATIC_LIB)
	$(CC) $(TEST_LDFLAGS) -o $@ $(TEST_OBJS) $(TEST_LDLIBS)

$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)/tests
	$(CC) $(TEST_CFLAGS) -c $< -o $@

# --- Install / Uninstall ----------------------------------------------------

install: all
	install -d $(PREFIX)/lib
	install -d $(PREFIX)/include/$(TARGET)
	install -m 644 $(STATIC_LIB) $(PREFIX)/lib/
	install -m 755 $(SHARED_LIB) $(PREFIX)/lib/
	ln -sf lib$(TARGET).so.$(VERSION)   $(PREFIX)/lib/lib$(TARGET).so.$(SOVERSION)
	ln -sf lib$(TARGET).so.$(SOVERSION) $(PREFIX)/lib/lib$(TARGET).so
	install -m 644 $(BOXER_HDRS) $(PREFIX)/include/$(TARGET)/

uninstall:
	rm -f $(PREFIX)/lib/lib$(TARGET).a
	rm -f $(PREFIX)/lib/lib$(TARGET).so*
	rm -rf $(PREFIX)/include/$(TARGET)

# --- Clean ------------------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR) $(TEST_TARGET)
