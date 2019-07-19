all: a.out

a.out: main.o
	gcc -Wall -o a.out main.o -lseccomp

main.o: main.c
	gcc -g -Wall -D DEBUG -D PTRACE -o main.o -c main.c

judge.o: judge.c
	gcc -g -Wall -o judge.o -c judge.c

callname.o: callname.c callname.h
	gcc -g -Wall -o callname.o -c callname.c

whitelist.o: whitelist.c whitelist.h
	gcc -g -Wall -o whitelist.o -c whitelist.c

clean:
	rm -f a.out main.o judge.o callname.o whitelist.o std.out err.out
