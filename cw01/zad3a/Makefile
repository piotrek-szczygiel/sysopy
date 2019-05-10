CC     := gcc
NAME   := find
LIBDIR := ../zad1
CFLAGS := -Wall -I$(LIBDIR) -O3
SRC    := $(NAME).c
OBJ    := $(NAME).o
RAPORT := raport3a.txt

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
	printf "static\n========\n" > $(RAPORT)
	make static
	make test
	printf "\n\nshared\n========\n" >> $(RAPORT)
	make shared
	make test
	printf "\n\ndynamic\n========\n" >> $(RAPORT)
	make dynamic
	make test
