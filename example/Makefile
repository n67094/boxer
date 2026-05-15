TARGET := example

CC = gcc
CFLAGS = -std=c99 -Wall
LDFLAGS = -lboxer

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
