CC     := gcc
NAME   := find
LIBDIR := ../zad1
O      := O3
CFLAGS := -Wall -I$(LIBDIR) -O$(O)
SRC    := $(NAME).c
OBJ    := $(NAME).o
RAPORT := raport3b.txt

.PHONY: static shared dynamic clean

static:
	$(CC) $(CFLAGS) -o $(NAME) $(SRC) $(LIBDIR)/libfind.a

shared:
	$(CC) $(CFLAGS) -o $(NAME) $(SRC) -L$(LIBDIR) -lfind

dynamic:
	$(CC) $(CFLAGS) -o $(NAME) $(SRC) -ldl -D DYNAMIC

clean:
	$(RM) $(OBJ) $(NAME)

test:
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	LD_LIBRARY_PATH=../zad1 ./$(NAME) $(shell cat test.txt)

test_all:
	printf "O3\n====\n" > $(RAPORT)
	make shared O=3
	make test
	printf "\n\n\nO2\n====\n" >> $(RAPORT)
	make shared O=2
	make test
	printf "\n\n\nO1\n====\n" >> $(RAPORT)
	make shared O=1
	make test
	printf "\n\n\nO0\n====\n" >> $(RAPORT)
	make shared O=0
	make test
	printf "\n\n\nOs\n====\n" >> $(RAPORT)
	make shared O=s
	make test
