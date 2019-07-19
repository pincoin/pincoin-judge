all: a.out

a.out: judge.o callname.o whitelist.o
	gcc -Wall -o a.out judge.o callname.o whitelist.o -lseccomp

judge.o: judge.c
	gcc -g -Wall -o judge.o -c judge.c

callname.o: callname.c callname.h
	gcc -g -Wall -o callname.o -c callname.c

whitelist.o: whitelist.c whitelist.h
	gcc -g -Wall -o whitelist.o -c whitelist.c

clean:
	rm -f a.out judge.o callname.o std.out err.out
