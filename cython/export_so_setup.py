
from setuptools import setup
from Cython.Build import cythonize

import os

# os.environ['CC'] = 'g++'
# os.environ['CXX'] = 'g++'

setup(
    name='export_so',
    ext_modules=cythonize("export_so.pyx", compiler_directives={
                          'language_level': "3"}),
    options={'build_ext': {'inplace': True}},
)
