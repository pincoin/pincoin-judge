from posix.types cimport pid_t

cdef extern from "judge.h":
    int examine(int argc, char *argv[])
    void run_solution(char **args)
    void watch_program(pid_t pid)
    int hello(char *s)

def py_hello(s):
    hello(s)
