CC=gcc
CFLAGS=-g -Wall -lfind -I../zad1
NAME=find

$(NAME): $(NAME).o
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).o

clean:
	$(RM) $(NAME) *.o raport*.txt

run: $(NAME)
	./$(NAME)
	
test: $(NAME)
	./$(NAME) $(shell cat test.txt)
