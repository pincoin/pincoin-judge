CC = /usr/bin/gcc
AR = /usr/bin/ar
CFLAGS = -Wall -DUSE_PTRACE -O2
LDFLAGS = -lseccomp
ARFLAGS = rcs

OBJECTS = main.o judge.o whitelist.o

all: sandbox libsandbox.a

sandbox: $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@ 

libsandbox.a: judge.o whitelist.o
	$(AR) $(ARFLAGS) $@ $^

c.o:
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -f sandbox *.a *.o
	rm -f *.out *.log
