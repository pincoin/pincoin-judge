CC = gcc

default: libexamples.a

libexamples.a: examples.o hello.o
	ar rcs $@ $^
    
examples.o: examples.c examples.h
	$(CC) -c $<

hello.o: hello.c hello.h
	$(CC) -c $<

clean:
	rm *.o *.a
