[![License](https://img.shields.io/github/license/f-squirrel/thread_pool)](https://github.com/f-squirrel/thread_pool/blob/master/LICENSE)
[![CI](https://github.com/f-squirrel/thread_pool/workflows/CI/badge.svg)](https://github.com/f-squirrel/thread_pool/actions)
[![Coverity Scan](https://img.shields.io/coverity/scan/21392.svg)](https://scan.coverity.com/projects/f-squirrel_thread_pool)
[![Coverage Status](https://coveralls.io/repos/github/f-squirrel/thread_pool/badge.svg?branch=master)](https://coveralls.io/github/f-squirrel/thread_pool?branch=master)

Thread Pool
===========


This is a C++ 11/14 thread pool implementation inspired by ["C++ Concurrency in Action: Practical Multithreading"](https://www.amazon.com/C-Concurrency-Action-Practical-Multithreading/dp/1933988770) by Anthony Williams.

The library is header-only and supposed to be compatible with any C++ 11/14 compiler.
<br>The only 3rd party is Google Test, and it is optional.

## Build ##
```sh
git clone --recursive https://github.com/f-squirrel/thread_pool.git
cd thread_pool
mkdir build && cd build
cmake -DBUILD_TESTING=ON ..
make
make test
make install
```

## Change the default namespace

The default namespace is `thread_pool`, there are two ways to change it:
* Via compilation flags `-DTHREAD_POOL_NAMESPACE_NAME=<your favorite namespace>`.
* Via changing `#define THREAD_POOL_NAMESPACE_NAME thread_pool` in ThreadPool.hpp.
