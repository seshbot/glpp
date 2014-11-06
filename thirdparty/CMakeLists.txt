
#
# GLFW
#

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw-3.0.4)

#
# GLEW
#

set(GLEW_LIBRARIES libglew_static CACHE INTERNAL "")

add_subdirectory(glew-cmake)

#
# GLM
#

set(GLM_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/glm-0.9.5.4 CACHE INTERNAL "")

add_subdirectory(glm-0.9.5.4)
