all: a.out

a.out: judge.o callname.o
	gcc -Wall -o a.out judge.o callname.o -lseccomp

judge.o: judge.c judge.h
	gcc -Wall -o judge.o -c judge.c

callname.o: callname.c callname.h
	gcc -Wall -o callname.o -c callname.c

clean:
	rm -f a.out judge.o callname.o std.out err.out
