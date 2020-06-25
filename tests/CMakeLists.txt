include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "*.cpp")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(TEST_COVERAGE)
    if(CMAKE_COMPILER_IS_GNUCXX)
        include(CodeCoverage)
        append_coverage_compiler_flags()
        setup_target_for_coverage_lcov(
            NAME coverage
            EXECUTABLE ThreadPoolTest
            EXCLUDE "/usr/*")
    endif()
endif()

add_executable(ThreadPoolTest ${SOURCES})
add_test(ThreadPoolTest ThreadPoolTest)
find_package(GTest REQUIRED)
target_link_libraries(ThreadPoolTest GTest::Main)
