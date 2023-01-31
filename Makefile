NAME	= thread_mult

DEBUG	= -g -O3
CC	= gcc -c
LD	= gcc
CFLAGS	= $(DEBUG) -D_GNU_SOURCE -W -Wall
LIBS	= -lpthread -lm
LDFLAGS = $(DEBUG)

OBJS = main.o matrices.o synchronize.o get_time.o

all : $(NAME)

$(NAME) : $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(NAME)

main.o		: main.c matrices.h get_time.h
matrices.o	: matrices.c matrices.h synchronize.h
synchronize.o	: synchronize.c synchronize.h
get_time.o	: get_time.c get_time.h
