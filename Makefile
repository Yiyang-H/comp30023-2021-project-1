CFLAGS=-Wall -ansi -std=c99

allocate: allocate.o cpu.o pqueue.o
	gcc $(CFLAGS) -o allocate allocate.o cpu.o pqueue.o -lm

allocate.o: allocate.c cpu.h pqueue.h
	gcc $(CFLAGS) -c allocate.c

cpu.o: cpu.c cpu.h pqueue.h
	gcc $(CFLAGS) -c cpu.c

pqueue.o: pqueue.c pqueue.h
	gcc $(CFLAGS) -c pqueue.c

clean: 
	rm -f allocate