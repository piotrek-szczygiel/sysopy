CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h
SRC     := error.c filter.c
OUT     := filter

.PHONY: $(OUT) all test clean
.DEFAULT: all

all: $(OUT)

$(OUT): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

test: $(OUT)
	./$(OUT) 1 block mona.pgm edge.fil mona_edge.pgm
	./$(OUT) 1 block mona.pgm gauss.fil mona_gauss.pgm
	./$(OUT) 1 block mona.pgm sharpen.fil mona_sharpen.pgm

clean:
	$(RM) $(OUT)
