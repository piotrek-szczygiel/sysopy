CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h
SRC     := piper.c error.c
OUT     := piper

$(OUT): $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

test: $(OUT)
	./$(OUT) command

clean:
	$(RM) $(OUT)
