CC      := gcc
CFLAGS  := -Wall -O3 -std=c99 -pthread
HEADERS := error.h
SRC     := error.c tcp.c

.PHONY: $(OUT) all client server test clean
.DEFAULT: all

all: client server

client: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) client.c -o client

server: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) server.c -o server

test: client server
	./server

clean:
	$(RM) client server
