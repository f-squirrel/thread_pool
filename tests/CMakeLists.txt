include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "*.cpp")

add_executable(ThreadPoolTest ${SOURCES})
add_test(ThreadPoolTest ThreadPoolTest)
find_package(GTest REQUIRED)
target_link_libraries(ThreadPoolTest GTest::Main)
