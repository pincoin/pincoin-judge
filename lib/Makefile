all: a.out

a.out: main.o judge.o whitelist.o
	gcc -Wall -o a.out main.o judge.o whitelist.o -lseccomp

main.o: main.c
	gcc -g -Wall -o main.o -c main.c

judge.o: judge.c judge.h
	gcc -g -Wall -D USE_PTRACE -o judge.o -c judge.c

whitelist.o: whitelist.c whitelist.h
	gcc -g -Wall -o whitelist.o -c whitelist.c

clean:
	rm -f a.out *.o
	rm -f *.out *.log
