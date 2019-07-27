cdef extern from "judge.h":
    int hello(int i, char c, char *s)

def py_hello(i, c, s):
    hello(i, c, s)
