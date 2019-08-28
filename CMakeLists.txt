cmake_minimum_required(VERSION 3.14)
project(kger)


set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE ON)

#add_subdirectory(includes/googletest-release-1.8.1
#        EXCLUDE_FROM_ALL)



add_subdirectory(libs)
add_subdirectory(app)
#include_directories(${graph_INCLUDE_DIRS} ${vocabulary_INCLUDE_DIRS} ${wikiparser_INCLUDE_DIRS})

#add_subdirectory(vocabulary)
#add_subdirectory(cpp)


