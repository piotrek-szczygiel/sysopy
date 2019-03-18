CC     := gcc
NAME   := files
CFLAGS := -Wall -O3
SRC    := $(NAME).c

.PHONY: clean

$(NAME): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME)

clean:
	$(RM) $(OBJ) $(NAME)
