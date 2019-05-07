CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h systemv_posix.h
SRC     := error.c systemv.c

.PHONY: all clean
.DEFAULT: all

all: trucker loader spawner

trucker: $(SRC) $(HEADERS) trucker.c
	$(CC) $(CFLAGS) $(SRC) trucker.c -o trucker

loader: $(SRC) $(HEADERS) loader.c
	$(CC) $(CFLAGS) $(SRC) loader.c -o loader

spawner: $(SRC) $(HEADERS) spawner.c
	$(CC) $(CFLAGS) $(SRC) spawner.c -o spawner

clean:
	$(RM) trucker loader spawner
