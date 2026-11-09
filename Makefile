# Authors: nsix
#
# Makefile for building boxer.
#
# Command examples:
#
#   make                              Build the project
#   make DEBUG=1                      Build with debug symbols
#   make SANITIZER=1                  Build with address sanitizer
#   make DEBUG=1 SANITIZER=1          Build with debug symbols and address sanitizer
#   make install PREFIX=/usr          Install the library to /usr/lib and headers to /usr/include/boxer
#   make uninstall                    Uninstall library and headers
#   make docs                         Generate documentation (using docgen)
#   make test                         Build tests (build/test/boxer_test)
#   make clean                        Clean build artifacts

UNAME_S := $(shell uname -s)

# Targets
TARGET := boxer

# Versioning
VERSION   := 0.0.0
SOVERSION := 0

# Boxer directories
BOXER_DOCS_DIR := docs
BOXER_SRC_DIR  := src
BOXER_INC_DIR  := include

# Build directories
TEST_DIR     := test
BUILD_DIR    := build
OBJ_DIR      := $(BUILD_DIR)/obj
LIB_OUT_DIR  := $(BUILD_DIR)/lib
TEST_BIN_DIR := $(BUILD_DIR)/test
DOCS_OUT_DIR := $(BUILD_DIR)/docs

# Library directories
LIB_DIR    := lib
PHYSFS_DIR := $(LIB_DIR)/physfs
SDL_GP_DIR := $(LIB_DIR)/SDL_gp
STB_DIR    := $(LIB_DIR)/stb

# Options
DEBUG     ?= 0
SANITIZER ?= 0

# Platform
ifeq ($(UNAME_S), Darwin)
PREFIX              ?= /usr/local
SHARED_EXT          := dylib
SHARED_LIB_NAME     := lib$(TARGET).$(VERSION).$(SHARED_EXT)
SHARED_LIB_VER_NAME := lib$(TARGET).$(SOVERSION).$(SHARED_EXT)
SHARED_LDFLAGS       = -dynamiclib \
                       -install_name $(INSTALL_LIB_DIR)/$(SHARED_LIB_VER_NAME) \
                       -compatibility_version $(SOVERSION) \
                       -current_version $(VERSION) \
                       -Wl,-U,_bxr_game_config -Wl,-U,_bxr_game_setup \
                       -Wl,-U,_bxr_game_update -Wl,-U,_bxr_game_render \
                       -Wl,-U,_bxr_game_teardown
PLATFORM_LDLIBS     := -framework Foundation -framework IOKit
PHYSFS_OBJC_SRCS    := $(PHYSFS_DIR)/physfs_platform_apple.m
else
PREFIX              ?= /usr
SHARED_EXT          := so
SHARED_LIB_NAME     := lib$(TARGET).$(SHARED_EXT).$(VERSION)
SHARED_LIB_VER_NAME := lib$(TARGET).$(SHARED_EXT).$(SOVERSION)
SHARED_LDFLAGS       = -shared -Wl,-soname,$(SHARED_LIB_VER_NAME)
PLATFORM_LDLIBS     :=
PHYSFS_OBJC_SRCS    :=
endif

SHARED_LNK_NAME := lib$(TARGET).$(SHARED_EXT)

# Install directories
INSTALL_LIB_DIR ?= $(PREFIX)/lib
INSTALL_INC_DIR ?= $(PREFIX)/include/$(TARGET)
DESTDIR         ?=

# Output library paths
STATIC_LIB     := $(LIB_OUT_DIR)/lib$(TARGET).a
SHARED_LIB     := $(LIB_OUT_DIR)/$(SHARED_LIB_NAME)
SHARED_LIB_VER := $(LIB_OUT_DIR)/$(SHARED_LIB_VER_NAME)
SHARED_LNK     := $(LIB_OUT_DIR)/$(SHARED_LNK_NAME)

# Compiler
CC = gcc
AR = ar

# Flags
LDLIBS  = -lSDL3 -lm $(PLATFORM_LDLIBS)
LDFLAGS =

# -fPIC: the same objects are used for the static and the shared library
# -MMD -MP: generate .d files so header changes trigger a rebuild
CFLAGS = -std=c99 -Wall -fPIC -MMD -MP -DBOXER_BUILD -D_GNU_SOURCE \
         -I$(PHYSFS_DIR) -I$(SDL_GP_DIR) -I$(BOXER_INC_DIR) -I$(STB_DIR)

ifeq ($(DEBUG), 1)
CFLAGS += -g -O0 -DDEBUG
else
CFLAGS += -O2
endif

ifeq ($(SANITIZER), 1)
CFLAGS  += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address
endif

# Source files
PHYSFS_SRCS := $(wildcard $(PHYSFS_DIR)/*.c)
BOXER_SRCS  := $(wildcard $(BOXER_SRC_DIR)/*.c)

# Object files (mirror the source tree under $(OBJ_DIR))
PHYSFS_OBJS := $(PHYSFS_SRCS:%.c=$(OBJ_DIR)/%.o) $(PHYSFS_OBJC_SRCS:%.m=$(OBJ_DIR)/%.o)
BOXER_OBJS  := $(BOXER_SRCS:%.c=$(OBJ_DIR)/%.o)
ALL_OBJS    := $(PHYSFS_OBJS) $(BOXER_OBJS)

# Docs files
BOXER_HDRS := $(wildcard $(BOXER_INC_DIR)/*.h)
BOXER_DOCS := $(patsubst $(BOXER_INC_DIR)/%.h, $(DOCS_OUT_DIR)/%.md, $(BOXER_HDRS))

# Test files
TEST_TARGET := $(TEST_BIN_DIR)/$(TARGET)_test
TEST_SRCS   := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS   := $(TEST_SRCS:%.c=$(OBJ_DIR)/%.o)
TEST_LDLIBS := $(STATIC_LIB) $(LDLIBS)

$(TEST_OBJS): CFLAGS += -I$(TEST_DIR) -I$(BOXER_SRC_DIR)

# Targets
.PHONY: all static shared test docs clean install uninstall
.DEFAULT_GOAL := all

all: static shared

static: $(STATIC_LIB)
shared: $(SHARED_LNK)

# Static library
$(STATIC_LIB): $(ALL_OBJS)
	@mkdir -p $(@D)
	$(AR) rcs $@ $^

# Shared library
$(SHARED_LIB): $(ALL_OBJS)
	@mkdir -p $(@D)
	$(CC) $(SHARED_LDFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Versioned symlinks: libboxer.so -> libboxer.so.0 -> libboxer.so.0.0.0
$(SHARED_LIB_VER): $(SHARED_LIB)
	ln -sf $(notdir $<) $@

$(SHARED_LNK): $(SHARED_LIB_VER)
	ln -sf $(notdir $<) $@

# Objects
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.m
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Documentation
docs: $(BOXER_DOCS)

$(DOCS_OUT_DIR)/%.md: $(BOXER_INC_DIR)/%.h
	@mkdir -p $(@D)
	node $(BOXER_DOCS_DIR)/docgen/docgen.js $< -o $@

# Test
test: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(STATIC_LIB)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $(TEST_OBJS) $(TEST_LDLIBS)

# Install / Uninstall
install: all
	install -d $(DESTDIR)$(INSTALL_LIB_DIR)
	install -d $(DESTDIR)$(INSTALL_INC_DIR)
	install -m 644 $(STATIC_LIB) $(DESTDIR)$(INSTALL_LIB_DIR)/
	install -m 755 $(SHARED_LIB) $(DESTDIR)$(INSTALL_LIB_DIR)/
	ln -sf $(SHARED_LIB_NAME)     $(DESTDIR)$(INSTALL_LIB_DIR)/$(SHARED_LIB_VER_NAME)
	ln -sf $(SHARED_LIB_VER_NAME) $(DESTDIR)$(INSTALL_LIB_DIR)/$(SHARED_LNK_NAME)
	install -m 644 $(BOXER_HDRS) $(DESTDIR)$(INSTALL_INC_DIR)/

uninstall:
	rm -f $(DESTDIR)$(INSTALL_LIB_DIR)/lib$(TARGET).a
	rm -f $(DESTDIR)$(INSTALL_LIB_DIR)/lib$(TARGET).*$(SHARED_EXT)*
	rm -rf $(DESTDIR)$(INSTALL_INC_DIR)

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Header dependencies
-include $(ALL_OBJS:.o=.d) $(TEST_OBJS:.o=.d)
