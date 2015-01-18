

CC = gcc

#CFLAGS	=	-g -Wall 
CFLAGS	=	-O3 -Wall

LIBFLAGS = 	-lm

all: main 

OBJS = graphgen.o graphdata.o heuristic.o stats.o options.o backtrack.o tester.o hamcycle.o
INCFILES = main.h graphgen.h graphdata.h heuristic.h stats.h options.h backtrack.h tester.h hamcycle.h

ALLOBJS = $(OBJS) main.o

$(ALLOBJS): $(INCFILES)

main:  $(ALLOBJS)
	$(CC) $(CFLAGS) $(ALLOBJS) -o main $(LIBFLAGS)

clean:;	rm -f *.o 

backup:; cp *.[ch] Makefile *.txt bak

tarfile:; tar -cvf prog.tar *.[ch] Makefile *.txt *.opt *.test
	gzip prog.tar
