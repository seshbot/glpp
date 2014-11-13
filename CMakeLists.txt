cmake_minimum_required (VERSION 2.8)

project (GlApp)

add_subdirectory(thirdparty)

include_directories(SYSTEM ${GLFW_SOURCE_DIR}/include ${GLFW_SOURCE_DIR}/deps)
include_directories(SYSTEM ${GLM_INCLUDES})
if (WIN32)
    include_directories(SYSTEM ${ANGLE_INCLUDES})
else ()
    include_directories(SYSTEM ${GLEW_INCLUDES})
endif ()

add_executable(glapp WIN32 main.cpp utils.cpp utils.h gl.cpp gl.h)

target_link_libraries(glapp glfw ${OPENGL_GLU_LIBRARY} ${GLFW_LIBRARIES})
if (WIN32)
    target_link_libraries(glapp ${ANGLE_LIBRARIES})
else ()
    target_link_libraries(glapp ${GLEW_LIBRARIES})
endif ()

INSTALL(TARGETS glapp DESTINATION bin)
