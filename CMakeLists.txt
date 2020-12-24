cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Remove for compiler-specific features
set(CMAKE_CXX_EXTENSIONS OFF)

string(APPEND CMAKE_CXX_FLAGS " -Wall")
string(APPEND CMAKE_CXX_FLAGS " -Wbuiltin-macro-redefined")
string(APPEND CMAKE_CXX_FLAGS " -pedantic")
string(APPEND CMAKE_CXX_FLAGS " -Werror")

# clangd completion
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(THREAD_POOL_NAMESPACE_NAME)
add_definitions(-DTHREAD_POOL_NAMESPACE_NAME=${THREAD_POOL_NAMESPACE_NAME})
endif()

option(TEST_COVERAGE "Test coverage" OFF)

project(thread_pool)
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

add_subdirectory(scripts)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/thread_pool DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
