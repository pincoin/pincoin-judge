all: a.out

a.out: main.o whitelist.o
	gcc -Wall -o a.out main.o whitelist.o -lseccomp

main.o: main.c main.h
	gcc -g -Wall -D USE_PTRACE -D TRACE_MEMORY -o main.o -c main.c

whitelist.o: whitelist.c whitelist.h
	gcc -g -Wall -o whitelist.o -c whitelist.c

clean:
	rm -f a.out *.o
	rm -f *.out *.log
