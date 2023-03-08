# Hook Lib

A small library designed to provide a simple API for hooking virtual functions in C++ objects. The same function can be hooked multiple times - this creates a chain of callbacks.

Callbacks have the ability to stop processing of the chain at the current function or cancel the execution of the original hooked function at the end.

# Build

Configure CMake and create a build directory with:
```
cmake -B build -S .
```
CMake will generate build scripts for you. To compile the program, type:
```
cmake -B build
```