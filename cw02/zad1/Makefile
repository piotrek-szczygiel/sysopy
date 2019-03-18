CC     := gcc
NAME   := files
CFLAGS := -Wall -O3
SRC    := $(NAME).c time.h
RAPORT := wyniki.txt

TEST   := test.txt
SMALL  := 2000
BIG    := 10000

.PHONY: clean

$(NAME): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME)

test: $(NAME)
	./$(NAME) $(shell sed "s/@/$(SMALL)/g; s/#/$(BIG)/g" $(TEST))

clean:
	$(RM) $(OBJ) $(NAME) $(RAPORT) t_*
