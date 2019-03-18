CC      := gcc
NAME    := list
CFLAGS  := -Wall -O3 -std=c99
SRC     := $(NAME).c error.c
HEADERS := error.h list.h

LIST    := list_dir.c

.PHONY: clean

$(NAME): $(SRC) $(HEADERS) $(LIST)
	$(CC) $(CFLAGS) $(SRC) $(LIST) -o $(NAME)

clean:
	$(RM) $(NAME)
