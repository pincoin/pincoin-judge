from libc.stdlib cimport malloc, free
from posix.types cimport pid_t

cdef extern from "judge.h":
    int examine(int argc, char *argv[])

    int hello(int i, char c, char *s)


def py_examine(args):
    cdef char **argv

    args = [b'a.out'] + [bytes(x, encoding='utf-8') for x in args]

    argv = <char**>malloc(sizeof(char*) * len(args))

    if argv is NULL:
        raise MemoryError()

    try:
        for i, s in enumerate(args):
            argv[i] = s

        examine(len(args), argv)
    finally:
        print('task done')
        free(argv)

def py_hello(i, c, s):
    print(hello(i, c, s))
