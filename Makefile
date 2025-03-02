CC = gcc

CFLAGS = -std=c17 -Wall -Wextra
LDFLAGS = `sdl2-config --cflags --libs` -lm

SRCS = $(wildcard *.c)

all: rc

rc: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o rc $(LDFLAGS)

clean:
	rm -f rc
