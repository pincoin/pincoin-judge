from posix.types cimport pid_t

cdef extern from "judge.h":
    int examine(int argc, char *argv[])
    void run_solution(char **args)
    void watch_program(pid_t pid)
    int hello(int i, char c, char *s)

def py_hello(i, c, s):
    hello(i, c, s)
