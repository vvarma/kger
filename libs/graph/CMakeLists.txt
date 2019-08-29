project(graph)

find_package(LibXml2 REQUIRED)
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS graph_io.proto)
#include_directories(${LIBXML2_INCLUDE_DIR})
add_definitions(${LIBXML2_DEFINITIONS})

add_library(${PROJECT_NAME} STATIC node.cpp unique_id.cpp owlparser.cpp nodebuilder.cpp graph_io.cpp ${PROTO_SRCS} ${PROTO_HDRS})
add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LIBXML2_INCLUDE_DIR}
        ${vocabulary_INCLUDE_DIRS}
        ${PROTOBUF_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        )
target_link_libraries(${PROJECT_NAME} ${LIBXML2_LIBRARIES} ${Protobuf_LIBRARIES})
SET(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
