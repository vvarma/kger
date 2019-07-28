cmake_minimum_required(VERSION 3.14)
project(kger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_subdirectory(includes/googletest-release-1.8.1
        EXCLUDE_FROM_ALL)

add_subdirectory(includes/cxxopts-master EXCLUDE_FROM_ALL)

add_subdirectory(includes/sentencepiece-master EXCLUDE_FROM_ALL)
target_include_directories(sentencepiece PRIVATE includes/sentencepiece-master/)

add_library(graph graph.cpp graph.h relation_parser.cpp relation_parser.h unique_id.cpp unique_id.h vocabulary.cpp vocabulary.h random_walker.cpp random_walker.h)

add_executable(extractor text_extractor.cpp)
target_link_libraries(extractor cxxopts graph)

add_executable(kger kger.cpp)
target_link_libraries(kger cxxopts graph sentencepiece)

add_executable(runGraphBuilderTest test_graph.cpp)
target_link_libraries(runGraphBuilderTest gtest_main graph)
