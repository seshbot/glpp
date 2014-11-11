CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

PROJECT( "angle" )

#
# Find EGL information
#

find_library(egl_LIBRARY_RELEASE libEGL ${CMAKE_CURRENT_SOURCE_DIR}/lib/Release)
find_library(egl_LIBRARY_DEBUG libEGL ${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug)

set(egl_LIBRARY optimized ${egl_LIBRARY_RELEASE} debug ${egl_LIBRARY_DEBUG})

find_path(egl_INCLUDE_DIR NAMES EGL/egl.h PATHS ${CMAKE_CURRENT_SOURCE_DIR}/include)

#
# Fild GLESv2 information
#

find_library(glesv2_LIBRARY_RELEASE libGLESv2 ${CMAKE_CURRENT_SOURCE_DIR}/lib/Release)
find_library(glesv2_LIBRARY_DEBUG libGLESv2 ${CMAKE_CURRENT_SOURCE_DIR}/lib/Debug)

set(glesv2_LIBRARY optimized ${glesv2_LIBRARY_RELEASE} debug ${glesv2_LIBRARY_DEBUG})

find_path(glesv2_INCLUDE_DIR NAMES GLES2/gl2.h PATHS ${CMAKE_CURRENT_SOURCE_DIR}/include)

list(APPEND angle_LIBRARIES ${egl_LIBRARY})
list(APPEND angle_LIBRARIES ${glesv2_LIBRARY})

#
# Export variables
#

set(ANGLE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")
set(ANGLE_LIBRARIES ${angle_LIBRARIES} CACHE INTERNAL "Dependencies of Angle")

set(ANGLE_EGL_INCLUDE_DIR ${egl_INCLUDE_DIR} CACHE INTERNAL "")
set(ANGLE_EGL_LIBRARY ${egl_LIBRARY} CACHE INTERNAL "")
set(ANGLE_GLESv2_INCLUDE_DIR ${glesv2_INCLUDE_DIR} CACHE INTERNAL "")
set(ANGLE_GLESv2_LIBRARY ${glesv2_LIBRARY} CACHE INTERNAL "")
