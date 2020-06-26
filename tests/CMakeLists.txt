include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "*.cpp")

if(TEST_COVERAGE)
    if(CMAKE_COMPILER_IS_GNUCXX)
        list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
        include(CodeCoverage)
        append_coverage_compiler_flags()
        setup_target_for_coverage_gcovr_html(
            NAME coverage
            EXECUTABLE ThreadPoolTest
            EXCLUDE "/usr/*")
    endif()
endif()

add_executable(ThreadPoolTest ${SOURCES})
add_test(ThreadPoolTest ThreadPoolTest)
find_package(GTest REQUIRED)
target_link_libraries(ThreadPoolTest GTest::Main)
