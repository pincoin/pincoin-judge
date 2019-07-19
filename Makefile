all: a.out

a.out: main.o whitelist.o
	gcc -Wall -o a.out main.o whitelist.o -lseccomp

main.o: main.c
	gcc -g -Wall -D PTRACE -o main.o -c main.c

whitelist.o: whitelist.c whitelist.h
	gcc -g -Wall -o whitelist.o -c whitelist.c

clean:
	rm -f a.out main.o judge.o whitelist.o std.out err.out
