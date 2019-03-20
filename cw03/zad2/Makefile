CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h
SRC     := monitor.c backup.c error.c
NAME    := monitor

$(NAME): $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME)

test: $(NAME)
	./$(NAME) list 15 mem

clean:
	$(RM) $(NAME)
