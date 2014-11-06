cmake_minimum_required (VERSION 2.8)

project (GlApp)

add_subdirectory(thirdparty)

include_directories(SYSTEM ${GLFW_SOURCE_DIR}/include
                    ${GLFW_SOURCE_DIR}/deps)

link_libraries(glfw ${OPENGL_GLU_LIBRARY})
link_libraries(${GLFW_LIBRARIES})

include_directories(SYSTEM ${glew_SOURCE_DIR}/include)
link_libraries(${GLEW_LIBRARIES})

include_directories(SYSTEM ${GLM_INCLUDES})

add_executable(glapp WIN32 main.cpp)
