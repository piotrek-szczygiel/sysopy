CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h list.h
SRC     := list.c error.c

.PHONY: clean

list_dir: $(HEADERS) $(SRC) list_dir.c
	$(CC) $(CFLAGS) $(SRC) list_dir.c -o list_dir

list_nftw: $(HEADERS) $(SRC) list_nftw.c
	$(CC) $(CFLAGS) $(SRC) list_nftw.c -o list_nftw

clean:
	$(RM) list_dir list_nftw
