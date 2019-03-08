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
	./$(NAME) table 2 search "/home/piotr/" "find.txt" "/home/piotr/fnd1.txt" search "/home/piotr/" "find2.txt" "/home/piotr/fnd2.txt" print 0 print 1
