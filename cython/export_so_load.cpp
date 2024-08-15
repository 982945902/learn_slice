#include <Python.h>
#include <dlfcn.h>

#include <iostream>

#include "export_so.h"

int main() {
  Py_Initialize();

  if (!Py_IsInitialized()) {
    printf("Error initializing Python interpreter\n");
    return 1;
  }

  void *handle = dlopen(
      "build/lib.macosx-13-arm64-cpython-311/export_so.cpython-311-darwin.so",
      RTLD_GLOBAL);
  if (!handle) {
    std::cerr << "Cannot open library: " << dlerror() << std::endl;
    return 1;
  }

  //   void (*display)(const char *);
  //   *(void **)(&display) = dlsym(handle, "display");

  //   const char *dlsym_error = dlerror();
  //   if (dlsym_error) {
  //     std::cerr << "Cannot load symbol 'add': " << dlsym_error << std::endl;
  //     dlclose(handle);
  //     return 1;
  //   }

  PyObject *(*PyInit_export_so)(void);
  *(void **)(&PyInit_export_so) = dlsym(handle, "PyInit_export_so");

  if (!PyInit_export_so) {
    std::cerr << "Cannot load symbol 'add': " << std::endl;
    dlclose(handle);
    return 1;
  }

  //   const char *dlsym_error = dlerror();
  //   if (dlsym_error) {
  //     std::cerr << "Cannot load symbol 'add': " << dlsym_error << std::endl;
  //     dlclose(handle);
  //     return 1;
  //   }
  PyObject *module = nullptr;
  module = PyInit_export_so();
  if (!module) {
    std::cerr << "Cannot load symbol 'add': " << std::endl;
    dlclose(handle);
    return 1;
  }

  std::cerr << "module" << std::endl;

  void (*display_int)(int);
  *(void **)(&display_int) = dlsym(handle, "display_int");

  //   dlsym_error = dlerror();
  //   if (dlsym_error) {
  //     std::cerr << "Cannot load symbol 'add': " << dlsym_error << std::endl;
  //     dlclose(handle);
  //     return 1;
  //   }

  if (!display_int) {
    std::cerr << "Cannot load symbol 'add': " << std::endl;
    dlclose(handle);
    return 1;
  }

  std::cerr << "display_int" << std::endl;

  PyRun_SimpleString("import export_so");

//   display_int(100);

  //   dlclose(handle);

  Py_Finalize();

  return 0;
}