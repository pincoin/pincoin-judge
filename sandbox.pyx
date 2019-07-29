from libc.stdlib cimport malloc, free
from posix.types cimport pid_t

cdef extern from "judge.h":
    int py_examine(int argc, char *argv[])

def examine(args):
    cdef char **argv

    args = [b'a.out'] + [bytes(x, encoding='utf-8') for x in args]

    argv = <char**>malloc(sizeof(char*) * len(args))

    if argv is NULL:
        raise MemoryError()

    try:
        for i, s in enumerate(args):
            argv[i] = s

        py_examine(len(args), argv)
    finally:
        print('task done')
        free(argv)

