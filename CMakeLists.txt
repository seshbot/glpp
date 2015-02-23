cmake_minimum_required (VERSION 2.8)

# TODO: use CMAKE_MODULE_PATH to find dependencies?

project (GLpp)

add_definitions(-DGL_GLEXT_PROTOTYPES)

add_subdirectory(thirdparty)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src/gles2)
add_subdirectory(src/glpp)
add_subdirectory(game)
