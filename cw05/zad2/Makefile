CC      := clang
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h
SRC     := error.c

default: master slave

master: master.c $(SRC)
	$(CC) $(CFLAGS) $(SRC) master.c -o master

slave: slave.c $(SRC)
	$(CC) $(CFLAGS) $(SRC) slave.c -o slave

test: master slave
	@./master fifo &
	@./slave fifo 1 &
	@./slave fifo 2 &
	@./slave fifo 3 &
	@./slave fifo 1 &
	@./slave fifo 4 &
	@./slave fifo 3 &
	@./slave fifo 2 &
	@./slave fifo 2 &
	@sleep 20

clean:
	@rm -f master slave fifo
