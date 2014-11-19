cmake_minimum_required (VERSION 2.8)

project( soil )

set( SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2 )
set( INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/SOIL2 )

include_directories( ${INCLUDE_DIR} )

file( GLOB SOIL_SRCS "${SRC_DIR}/*.c" )

add_library( soil STATIC ${SOIL_SRCS} )
set_source_files_properties( ${SOIL_SRCS} PROPERTIES LANGUAGE CXX )

