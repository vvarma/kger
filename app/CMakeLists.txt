project("noderunner")
set(CMAKE_CXX_STANDARD 17)
#add_subdirectory(../includes/ EXCLUDE_FROM_ALL)
add_subdirectory(../includes/cxxopts-master EXCLUDE_FROM_ALL cxxopts)
add_executable(${PROJECT_NAME} noderunner.cpp)
target_link_libraries(noderunner cxxopts libs::wikiparser libs::models libs::graph libs::vocabulary)

add_executable(wiki_to_graph wiki_to_graph.cpp)
target_link_libraries(wiki_to_graph cxxopts libs::vocabulary libs::graph libs::wikiparser)
