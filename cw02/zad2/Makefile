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
	$(RM) -rf list_dir list_nftw test_tree

test: list_dir list_nftw
	mkdir -p test_tree/subdirectory
	touch -d "2019-01-01 12:00:00" test_tree/file1
	ln -s test_tree/file1 test_tree/subdirectory/link1
	touch -d "2019-01-02 05:05:05" test_tree/file2

	./list_dir test_tree "=" "2019-01-02 05:05:05"
	./list_nftw test_tree "=" "2019-01-02 05:05:05"

	./list_dir test_tree ">" "2019-01-01 00:00:00"
	./list_nftw test_tree ">" "2019-01-01 00:00:00"
