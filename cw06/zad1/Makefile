CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h utils.h message.h systemv_posix.h
SRC     := error.c utils.c message.c systemv.c

.PHONY: all clean
.DEFAULT: all

all: chat_client chat_server

chat_client: tui.c tui.h client.c $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) tui.c client.c $(SRC) -lncurses -o chat_client

chat_server: server.c $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) server.c $(SRC) -o chat_server

clean:
	$(RM) chat_client chat_server
