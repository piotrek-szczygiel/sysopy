CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h
SRC     := client.c error.c
OUT     := client

$(OUT): $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -lncurses -o $(OUT)

test: $(OUT)
	./$(OUT)

clean:
	$(RM) $(OUT)
