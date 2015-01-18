#
#     ``THIS SOURCE CODE IS SUPPLIED  ``AS IS'' WITHOUT WARRANTY OF ANY KIND,
#     AND ITS AUTHOR AND THE JOURNAL OF ARTIFICIAL INTELLIGENCE RESEARCH
#     (JAIR) AND JAIR'S PUBLISHERS AND DISTRIBUTORS, DISCLAIM ANY AND ALL
#     WARRANTIES, INCLUDING BUT NOT LIMITED TO ANY IMPLIED
#     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND
#     ANY WARRANTIES OR NON INFRINGEMENT.  THE USER ASSUMES ALL LIABILITY AND
#     RESPONSIBILITY FOR USE OF THIS SOURCE CODE, AND NEITHER THE AUTHOR NOR
#     JAIR, NOR JAIR'S PUBLISHERS AND DISTRIBUTORS, WILL BE LIABLE FOR
#     DAMAGES OF ANY KIND RESULTING FROM ITS USE.  Without limiting the
#     generality of the foregoing, neither the author, nor JAIR, nor JAIR's
#     publishers and distributors, warrant that the Source Code will be
#     error-free, will operate without interruption, or will meet the needs
#     of the user.''
#

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
