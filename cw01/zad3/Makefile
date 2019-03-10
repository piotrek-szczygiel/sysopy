CC     := gcc
NAME   := find
LIBDIR := ../zad1
O      := O3
CFLAGS := -Wall -I$(LIBDIR) -$(O)
SRC    := $(NAME).c
OBJ    := $(NAME).o

.PHONY: static shared dynamic clean

static:
	$(CC) $(CFLAGS) -o $(NAME) $(SRC) $(LIBDIR)/libfind.a

shared:
	$(CC) $(CFLAGS) -o $(NAME) $(SRC) -L$(LIBDIR) -lfind

dynamic:
	$(CC) $(CFLAGS) -o $(NAME) $(SRC) -ldl -D DYNAMIC
	
clean:
	$(RM) $(OBJ) $(NAME) raport.txt

test:
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	LD_LIBRARY_PATH=../zad1 ./$(NAME) $(shell cat test.txt)
