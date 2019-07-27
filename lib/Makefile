CC = /usr/bin/gcc
CFLAGS = -Wall -DUSE_PTRACE -O2
LDFLAGS = -lseccomp

TARGET = judge
OBJECTS = main.o judge.o whitelist.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@ 

c.o:
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -f *.out *.log
