cdef extern from "examples.h":
    void example(const char *name)

def py_hello(name: bytes) -> None:
    example(name)
