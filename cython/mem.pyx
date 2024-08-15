from cpython.mem cimport PyMem_Malloc, PyMem_Free

cdef class Foo:
    cdef void *data
    def __init__(self,size_t size):
        self.data = PyMem_Malloc(size)
        if not self.data:
            raise MemoryError()

    def __dealloc__(self):
        PyMem_Free(self.data)