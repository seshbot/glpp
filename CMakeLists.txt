cmake_minimum_required (VERSION 2.8)

# TODO: use CMAKE_MODULE_PATH to find dependencies?

project (GlApp)

add_subdirectory(thirdparty)

include_directories(SYSTEM ${GLFW_SOURCE_DIR}/include ${GLFW_SOURCE_DIR}/deps)
include_directories(SYSTEM ${SOIL_INCLUDES})
include_directories(SYSTEM ${GLM_INCLUDES})
include_directories(SYSTEM ${ASSIMP_INCLUDES})
include_directories(${CMAKE_SOURCE_DIR}/include)
if (WIN32)
    include_directories(SYSTEM ${ANGLE_INCLUDES})
    
    add_definitions(-DANGLE_BUILD)
else ()
    include_directories(SYSTEM ${GLEW_INCLUDES})
endif ()

set(SOURCE
	main.cpp
	game.cpp    game.h
	src/utils.cpp   include/glpp/utils.h
	src/input.cpp   include/glpp/input.h
	src/glpp.cpp    include/glpp/glpp.h
	src/gles2/commands.cpp
	src/gles2/extensions.cpp
	src/gles2/angle_extension_macros.h
	include/glpp/gles2/commands.h
	include/glpp/gles2/extensions.h
	include/glpp/gles2/extensions_enums.h
	include/glpp/gles2/enums.h
	include/glpp/types.h)

set(SHADERS
	shaders/2d.vert shaders/2d.frag
	shaders/3d.vert shaders/3d.frag
    shaders/post.vert shaders/post.frag
    shaders/sprite.vert shaders/sprite.frag)

add_executable(glapp WIN32 CMakeLists.txt ${SOURCE} ${SHADERS})

source_group(Shaders FILES ${SHADERS})

target_link_libraries(glapp glfw soil ${OPENGL_GLU_LIBRARY} ${GLFW_LIBRARIES} ${ASSIMP_LIBRARIES})
if (WIN32)
    target_link_libraries(glapp ${ANGLE_LIBRARIES})
else ()
    target_link_libraries(glapp ${GLEW_LIBRARIES})
endif ()

add_custom_command(TARGET glapp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders)            # $<TARGET_FILE_DIR:glapp>/shaders)

install(TARGETS glapp DESTINATION bin)
