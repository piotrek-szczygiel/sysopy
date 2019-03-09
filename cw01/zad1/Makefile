CC      := gcc
NAME    := libfind
SNAME   := $(NAME).a
DNAME   := $(NAME).so
SRC     := find.c
OBJ     := find.o
CFLAGS  := -std=c99 -pedantic -Wall -W -Werror -g3 -fPIC

.PHONY: all clean fclean re

all: $(SNAME) $(DNAME)

$(SNAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(DNAME): LDFLAGS += -shared
$(DNAME): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) $(OBJ) $(SNAME) $(DNAME)

re: clean all
