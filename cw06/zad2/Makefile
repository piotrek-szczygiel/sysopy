CC      := gcc
CFLAGS  := -Wall -O3 -std=c99 -I../zad1
HEADERS := ../zad1/error.h ../zad1/utils.h ../zad1/message.h ../zad1/systemv_posix.h
SRC     := ../zad1/error.c ../zad1/utils.c ../zad1/message.c posix.c

.PHONY: all test clean
.DEFAULT: all

all: chat_client chat_server

chat_client: ../zad1/tui.c ../zad1/tui.h ../zad1/client.c $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) ../zad1/tui.c ../zad1/client.c $(SRC) -lrt -lncurses -o chat_client

chat_server: ../zad1/server.c $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) ../zad1/server.c $(SRC) -lrt -o chat_server

test: chat_client chat_server
	./chat_server

clean:
	$(RM) chat_client chat_server
