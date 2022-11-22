COMPILER = gcc -Wall -pthread
EXES = search

compile: dmood_hw4.c
	${COMPILER} dmood_hw4.c -o dmood_hw4

%.o: %.c %.h  makefile
	${COMPILER} $< -c 

remove:
	rm -f *.o *~ ${EXES} ${CFILES}
