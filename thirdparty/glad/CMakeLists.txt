cmake_minimum_required( VERSION 2.6 )
PROJECT( Glad )

set(SOURCE
  src/glad.c
  include/glad/glad.h
  include/KHR/khrplatform.h)

add_library(glad ${SOURCE})

target_include_directories(glad PRIVATE include)

set(GLAD_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "")
