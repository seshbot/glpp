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

set(SOURCE
	main.cpp
	utils.cpp utils.h
	gl.cpp    gl.h
	input.cpp input.h)

set(SHADERS
	shaders/2d.frag shaders/2d.vert)

add_executable(glapp WIN32 ${SOURCE} ${SHADERS})

source_group(Shaders FILES ${SHADERS})

target_link_libraries(glapp glfw ${OPENGL_GLU_LIBRARY} ${GLFW_LIBRARIES})
if (WIN32)
    target_link_libraries(glapp ${ANGLE_LIBRARIES})
else ()
    target_link_libraries(glapp ${GLEW_LIBRARIES})
endif ()

add_custom_command(TARGET glapp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders)            # $<TARGET_FILE_DIR:glapp>/shaders)

INSTALL(TARGETS glapp DESTINATION bin)
