PROG = main
OBJS = main.o explor.o search.o
CFLAGS = -Wall
LIBS = -lpthread -lreadline
CC = gcc

all: $(PROG)

$(PROG): $(OBJS)

clean: 	
	rm -f *.o main