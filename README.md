Thread Pool
===========


This is a C++ 11/14 thread pool implementation inspired by "C++ Concurrency in Action: Practical Multithreading" by Anthony Williams.

The library is header-only and supposed to be compatible with any C++ 11/14 compiler.
<br>The only 3rd party is Google Test, and it is optional.

## Build ##
```sh
cd thread_pool
mkdir build && cd build
cmake -DBUILD_TESTING=ON ..
make
make test
make install
```
