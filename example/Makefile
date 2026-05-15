TARGET := example

CC = gcc
CFLAGS = -std=c99 -Wall
LDFLAGS = -lboxer -lSDL3

ASSETS_DIR := assets
ASSETS_ZIP := $(ASSETS_DIR).zip

.PHONY: all clean

all: $(TARGET) $(ASSETS_ZIP)

$(ASSETS_ZIP): $(ASSETS_DIR)
	zip -r $@ $<

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(ASSETS_ZIP)
