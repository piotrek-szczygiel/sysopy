D       := ../zad1

CC      := gcc
CFLAGS  := -Wall -O3 -std=c99 -I$(D) -DPOSIX -lrt -pthread
HEADERS := $(D)/error.h $(D)/utils.h $(D)/queue.h $(D)/shared.h $(D)/pack.h
SRC     := $(D)/error.c $(D)/utils.c $(D)/queue.c $(D)/pack.c posix.c

.PHONY: all clean
.DEFAULT: all

all: trucker loader spawner

trucker: $(SRC) $(HEADERS) $(D)/trucker.c
	$(CC) $(CFLAGS) $(SRC) $(D)/trucker.c -o trucker

loader: $(SRC) $(HEADERS) $(D)/loader.c
	$(CC) $(CFLAGS) $(SRC) $(D)/loader.c -o loader

spawner: $(SRC) $(HEADERS) $(D)/spawner.c
	$(CC) $(CFLAGS) $(SRC) $(D)/spawner.c -o spawner

clean:
	$(RM) trucker loader spawner
