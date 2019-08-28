project(vocabulary)

SET(SENTENCEPICE_SRC ${CMAKE_SOURCE_DIR}/includes/sentencepiece-master)
#add_subdirectory( sentencepiece)
#target_include_directories(sentencepiece PRIVATE ${SENTENCEPICE_SRC})

add_library(${PROJECT_NAME} STATIC vocabulary.cpp)
add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${SENTENCEPICE_SRC}
        )
target_link_libraries(${PROJECT_NAME} sentencepiece)
#add_library(vocabulary vocabulary.h vocabulary.cpp)
SET(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)