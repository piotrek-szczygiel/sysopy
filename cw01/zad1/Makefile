CC=gcc
CFLAGS=-fPIC -O3 -g -Wall -Werror
NAME=find

LIBSO=lib$(NAME).so
LIBA=lib$(NAME).a

lib: $(LIBSO) $(LIBA)

$(LIBSO): $(NAME).o
	$(CC) $(CFLAGS) -shared -o $(LIBSO) $(NAME).o

$(LIBA): $(NAME).o
	$(AR) $(ARFLAGS) $(LIBA) $(NAME).o

clean:
	$(RM) *.o *.so* *.a

install: lib
	sudo cp $(LIBSO) /usr/lib
	sudo cp $(LIBA) /usr/lib
