CC      := gcc
NAME    := libfind
SNAME   := $(NAME).a
DNAME   := $(NAME).so
SRC     := find.c
OBJ     := $(SRC:.c=.o)
CFLAGS  := -Wall -fPIC -O3

.PHONY: all clean clean rebuild

all: $(SNAME) $(DNAME)

$(SNAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(DNAME): LDFLAGS += -shared
$(DNAME): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) $(OBJ) $(SNAME) $(DNAME)

rebuild: clean all
