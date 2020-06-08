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

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "*.cpp")

add_executable(ThreadPoolTest ${SOURCES})
add_test(ThreadPoolTest ThreadPoolTest)
find_package(GTest REQUIRED)
target_link_libraries(ThreadPoolTest GTest::Main)
