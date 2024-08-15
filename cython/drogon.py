# %env CC=/opt/homebrew/Cellar/gcc@11/11.4.0/bin/g++-11
# %env CXX=/opt/homebrew/Cellar/gcc@11/11.4.0/bin/g++-11
# %env LD_LIBRARY_PATH=/Users/lishuo121/workspace/drogon/install/lib/
# %env DYLD_LIBRARY_PATH=/Users/lishuo121/workspace/drogon/install/lib/
# %env RPATH=/Users/lishuo121/workspace/drogon/install/lib/
# %env DYLD_FALLBACK_LIBRARY_PATH=/Users/lishuo121/workspace/drogon/install/lib/

import os

os.environ["CC"] = "/usr/bin/g++"
os.environ["CXX"] = "/usr/bin/g++"
os.environ["CFLAGS"] = "-std=c++20  -O2"
os.environ["CXXFLAGS"] = "-std=c++20  -O2"

import pyximport
pyximport.install(setup_args={
            #'language': 'c++',
            "include_dirs": ["/Users/lishuo121/workspace/drogon/install/include","/opt/homebrew/Cellar/jsoncpp/1.9.5/include/"],
            # 'extra_compile_args': ["-Wno-cpp", "-Wno-unused-function", "-O2", "-march=native", '-stdlib=libc++', '-std=c++21'],
            # 'extra_link_args': ["-O2", "-march=native", '-stdlib=libc++'],
    })
import ctypes
from ctypes import CDLL


CDLL("/Users/lishuo121/workspace/drogon/install/lib/libtrantor.1.dylib", ctypes.RTLD_GLOBAL)
CDLL("/Users/lishuo121/workspace/drogon/install/lib/libdrogon.1.dylib", ctypes.RTLD_GLOBAL)


import esse
import asyncio
import time

app = esse.App()


@app.GET(path="/test")
async def test(req):
    #await asyncio.sleep(0.01)
    return "test"

# @app.GET(path="/test")
# def test(req):
#     time.sleep(0.01)
#     return "test"

app.run("0.0.0.0",8888)

# import time
# time.sleep(30)