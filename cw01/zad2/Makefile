CC=gcc
CFLAGS=-O3 -g -Wall -lfind -I../zad1
NAME=find

TEST=logfile "raport.log" table 1 clock_start search "/" "*" "/tmp/find_test_1" clock_stop "big_search"

$(NAME): $(NAME).o
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).o

clean:
	$(RM) $(NAME) *.o

run: $(NAME)
	./$(NAME)
	
test: $(NAME)
	./$(NAME) $(TEST)
