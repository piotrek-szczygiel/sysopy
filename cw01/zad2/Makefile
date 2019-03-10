CC     := gcc
CFLAGS := -std=c99 -Wall -Werror -I../zad1 -L../zad1 -lfind -O3
NAME   := find
SRC    := find.c

$(NAME): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME)

clean:
	$(RM) $(NAME) *.o raport.txt
	
test: $(NAME)
	echo 3 | sudo tee /proc/sys/vm/drop_caches
	LD_LIBRARY_PATH=../zad1 ./$(NAME) $(shell cat test.txt)
