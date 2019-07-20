cmake_minimum_required(VERSION 3.14)
project(kger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
add_subdirectory(includes/googletest-release-1.8.1
        EXCLUDE_FROM_ALL)


add_executable(kger main.cpp graph.cpp graph.h relation_parser.cpp relation_parser.h)

add_executable(runGraphBuilderTest test_graph.cpp graph.cpp graph.h)

target_link_libraries(runGraphBuilderTest gtest_main)
