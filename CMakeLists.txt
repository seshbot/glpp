cmake_minimum_required (VERSION 2.8)

project (GlApp)

add_subdirectory(thirdparty)

include_directories(SYSTEM ${GLFW_SOURCE_DIR}/include ${GLFW_SOURCE_DIR}/deps)
include_directories(SYSTEM ${GLM_INCLUDES})
if (WIN32)
    include_directories(SYSTEM ${GLEW_INCLUDES})
    include_directories(SYSTEM ${ANGLE_INCLUDES})
endif ()

add_executable(glapp WIN32 main.cpp)

target_link_libraries(glapp glfw ${OPENGL_GLU_LIBRARY})
target_link_libraries(glapp ${GLFW_LIBRARIES})
if (WIN32)
    target_link_libraries(glapp ${GLEW_LIBRARIES})
    target_link_libraries(glapp ${ANGLE_LIBRARIES})
endif ()

INSTALL(TARGETS glapp DESTINATION bin)
