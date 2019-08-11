cmake_minimum_required(VERSION 3.14)
project(kger)

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

list(APPEND CMAKE_PREFIX_PATH includes/libtorch)
add_subdirectory(includes/googletest-release-1.8.1
        EXCLUDE_FROM_ALL)

find_package(Torch REQUIRED)

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

add_executable(noderunner owlparser.h owlparser.cpp unique_id.h unique_id.cpp node.cpp node.h nodebuilder.cpp nodebuilder.h owlparser.h noderunner.cpp relation_parser.h relation_parser.cpp vocabulary.cpp vocabulary.h GraphDataset.cpp GraphDataset.h EntityModeller.cpp EntityModeller.h)
target_link_libraries(noderunner ${LIBXML2_LIBRARIES} cxxopts sentencepiece "${TORCH_LIBRARIES}")

add_executable(testtorch testtorch.cpp)
target_link_libraries(testtorch  ${LIBXML2_LIBRARIES} cxxopts sentencepiece "${TORCH_LIBRARIES}")
