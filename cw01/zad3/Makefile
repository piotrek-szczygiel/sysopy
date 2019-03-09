CC     := gcc
NAME   := find
LIBDIR := ../zad1
CFLAGS := -Wall -I$(LIBDIR)
SRC    := $(NAME).c
OBJ    := $(NAME).o

.PHONY: static shared dynamic clean

default: shared

static:
	$(CC) $(CFLAGS) -o $(NAME) $(SRC) $(LIBDIR)/libfind.a

shared:
	$(CC) $(CFLAGS) -o $(NAME) $(SRC) -L$(LIBDIR) -lfind

dynamic:
	$(CC) $(CFLAGS) -o $(NAME) $(SRC) -ldl -D DYNAMIC
	
clean:
	$(RM) $(OBJ) $(NAME)

test: $(NAME)
	./$(NAME) $(shell cat test.txt)
