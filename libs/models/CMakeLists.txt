project(models)
list(APPEND CMAKE_PREFIX_PATH ../../includes/libtorch)
find_package(Torch REQUIRED)
add_library(${PROJECT_NAME} STATIC model.cpp trainer.cpp)
add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )
add_library(model_internal EntityModeller.cpp GraphDataset.cpp)
target_link_libraries(model_internal "${TORCH_LIBRARIES}")
target_include_directories(model_internal PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} model_internal)

SET(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
