project(models)
list(APPEND CMAKE_PREFIX_PATH ../../includes/libtorch)
find_package(Torch REQUIRED)
add_library(${PROJECT_NAME} STATIC model.cpp trainer.cpp EntityModeller.cpp GraphDataset.cpp)
add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
target_link_libraries(${PROJECT_NAME} ${TORCH_LIBRARIES})

SET(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
