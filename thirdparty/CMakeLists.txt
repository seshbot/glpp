
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw-3.0.4)
add_subdirectory(glew-cmake)
add_subdirectory(glm-0.9.5.4)

set(GLEW_LIBRARIES glew CACHE INTERNAL "")
set(GLM_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/glm-0.9.5.4 CACHE INTERNAL "")
