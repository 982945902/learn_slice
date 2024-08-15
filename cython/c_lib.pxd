cdef extern from "c_lib.h":
    void c_print()

cdef inline print():
    c_print()