CC = gcc
INCLUDE_DIR = -Iinclude
CFLAGS = $(INCLUDE_DIR) -std=c17 -Wall -Wextra
DEBUG = -DDEBUG
LDFLAGS = `sdl2-config --cflags --libs` -lm

SRCS = $(wildcard src/*.c)

all: rc

rc: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o rc $(LDFLAGS)

debug: $(SRCS)
	$(CC) $(DEBUG) $(CFLAGS) $(SRCS) -o rc $(LDFLAGS)

clean:
	rm -f rc
