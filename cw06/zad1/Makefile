CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h utils.h
SRC     := error.c utils.c

.PHONY: all test clean
.DEFAULT: all

all: chat_client chat_server

chat_client: client.c $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) client.c $(SRC) -lncurses -o chat_client

chat_server: server.c $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) server.c $(SRC) -o chat_server

test: chat_client chat_server
	./chat_server

clean:
	$(RM) chat_client chat_server
