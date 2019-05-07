CC      := gcc
CFLAGS  := -Wall -O3 -std=c99 -lrt
HEADERS := error.h
SRC     := error.c

.PHONY: all clean
.DEFAULT: all

all: client server

client: $(HEADERS) $(SRC) client.c
	$(CC) $(CFLAGS) $(SRC) client.c -o client

server: $(HEADERS) $(SRC) server.c
	$(CC) $(CFLAGS) $(SRC) server.c -o server

clean:
	$(RM) client server
