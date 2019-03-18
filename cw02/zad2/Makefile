CC     := gcc
NAME   := list
CFLAGS := -Wall -O3
SRC    := $(NAME).c

.PHONY: clean

$(NAME): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME)

clean:
	$(RM) $(NAME)
