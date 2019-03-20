CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h
SRC     := monitor.c backup.c error.c
NAME    := monitor

$(NAME): $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME)

tester: tester.c
	$(CC) $(CFLAGS) tester.c -o tester

clean:
	$(RM) -rf $(NAME) tester files archive

test: $(NAME) tester
	mkdir "files"
	echo "files/file_1 5" > files/list1
	echo "files/file_2 10" >> files/list1
	echo "files/file_3 3" > files/list2
	echo "files/file_4 6" >> files/list2

	./tester files/file_1 1 10 16 &
	./tester files/file_2 3 7 32 &
	./tester files/file_3 2 8 8 &
	./tester files/file_4 5 15 32 &

	./monitor files/list1 30 exec &
	sleep 1
	./monitor files/list2 30 mem &
	sleep 31
	pkill tester
