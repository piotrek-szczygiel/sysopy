CC      := gcc
CFLAGS  := -Wall -O3 -std=c99

sender: sender.c
	$(CC) $(CFLAGS) sender.c error.c -o sender

catcher: catcher.c
	$(CC) $(CFLAGS) catcher.c error.c -o catcher

clean:
	@rm -f sender catcher

test: sender catcher
	@bash test.sh kill
	@bash test.sh queue
	@bash test.sh rt
