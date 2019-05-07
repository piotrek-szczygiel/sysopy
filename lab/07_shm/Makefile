CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h
SRC     := error.c

.PHONY: all clean
.DEFAULT: all

all: p_client p_server v_client v_server

p_client: $(HEADERS) $(SRC) p_client.c
	$(CC) $(CFLAGS) $(SRC) -lrt p_client.c -o p_client

p_server: $(HEADERS) $(SRC) p_server.c
	$(CC) $(CFLAGS) $(SRC) -lrt p_server.c -o p_server

v_client: $(HEADERS) $(SRC) v_client.c
	$(CC) $(CFLAGS) $(SRC) v_client.c -o v_client

v_server: $(HEADERS) $(SRC) v_server.c
	$(CC) $(CFLAGS) $(SRC) v_server.c -o v_server

clean:
	$(RM) p_client p_server v_client v_server
