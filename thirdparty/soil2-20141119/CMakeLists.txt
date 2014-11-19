cmake_minimum_required (VERSION 2.8)

project( soil CXX )

set( SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2 )
set( INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2 )

include_directories( ${INCLUDE_DIR} )

file( GLOB SOIL_SRCS "${SRC_DIR}/*.c" )

add_library( soil STATIC ${SOIL_SRCS} )

if (WIN32)
  set_source_files_properties( ${SOIL_SRCS} PROPERTIES LANGUAGE CXX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")
endif ()