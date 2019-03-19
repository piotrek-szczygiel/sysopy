CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h
SRC     := ls.c error.c
OUT     := ls

$(OUT): $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

test: $(OUT)
	./$(OUT) /boot

clean:
	$(RM) $(OUT)
