PROG = grepr
OBJS = main.o explor.o search.o
CFLAGS = -Wall
LIBS = -lpthread 
CC = gcc

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(LIBS)
clean: 
	rm -f *.o main
clear: 	
	rm -f *.o main