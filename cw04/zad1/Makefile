CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h
SRC     := halt.c error.c
OUT     := halt

$(OUT): $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

test: $(OUT)
	./$(OUT)

clean:
	$(RM) $(OUT)
