cmake_minimum_required (VERSION 2.8)

# TODO: use CMAKE_MODULE_PATH to find dependencies?

project (GLpp)

add_definitions(-DGL_GLEXT_PROTOTYPES)

if (WIN32)
    add_definitions(-DGLPP_USING_ANGLE)
    add_definitions(-DGLPP_USING_GLES2)
endif ()

add_subdirectory(thirdparty)
include_directories(${CMAKE_SOURCE_DIR}/include)

if (WIN32)
  add_subdirectory(src/gles2)
else ()
  add_subdirectory(src/gl2)
endif ()

add_subdirectory(src/glpp)
add_subdirectory(game)
