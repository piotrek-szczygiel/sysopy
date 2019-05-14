CC      := gcc
CFLAGS  := -Wall -O3 -std=c99 -lpthread

.PHONY: all

all: pthread

pthread: pthread.c error.c
	$(CC) $(CFLAGS) pthread.c error.c -o pthread

clean:
	$(RM) pthread
