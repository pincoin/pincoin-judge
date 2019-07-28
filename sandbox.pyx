from posix.types cimport pid_t

cdef extern from "judge.h":
    int examine(int argc, char *argv[])

    int hello(int i, char c, char *s)

def py_hello(i, c, s):
    print(hello(i, c, s))
