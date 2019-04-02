CC      := gcc
CFLAGS  := -Wall -O3 -std=c99
HEADERS := error.h
SRC     := popen.c error.c
OUT     := popen

$(OUT): $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

test: $(OUT)
	./$(OUT) "ls -l" "wc -l" "output1"
	./$(OUT) "ps -aux" "grep kde" "output2"

clean:
	$(RM) $(OUT) output1 output2
