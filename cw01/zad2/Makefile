CC=gcc
CFLAGS=-O3 -g -Wall -lfind -I../zad1
NAME=find

$(NAME): $(NAME).o
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).o

clean:
	$(RM) $(NAME) *.o

run: $(NAME)
	./$(NAME)
	
test: $(NAME)
	./$(NAME) $(shell cat test.txt)
