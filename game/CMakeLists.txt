
set(SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/game.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/game.h)

set(SHADERS
  ${PROJECT_SOURCE_DIR}/shaders/2d.vert ${PROJECT_SOURCE_DIR}/shaders/2d.frag
  ${PROJECT_SOURCE_DIR}/shaders/3d.vert ${PROJECT_SOURCE_DIR}/shaders/3d.frag
  ${PROJECT_SOURCE_DIR}/shaders/post.vert ${PROJECT_SOURCE_DIR}/shaders/post.frag
  ${PROJECT_SOURCE_DIR}/shaders/sprite.vert ${PROJECT_SOURCE_DIR}/shaders/sprite.frag)

add_executable(glgame WIN32 CMakeLists.txt ${SOURCE} ${SHADERS})

source_group(Shaders FILES ${SHADERS})

target_include_directories(glgame SYSTEM PRIVATE ${GLFW_SOURCE_DIR}/include ${GLFW_SOURCE_DIR}/deps)
target_include_directories(glgame SYSTEM PRIVATE ${ASSIMP_INCLUDES})

target_link_libraries(glgame PRIVATE glfw ${OPENGL_GLU_LIBRARY} ${GLFW_LIBRARIES} ${ASSIMP_LIBRARIES})
target_link_libraries(glgame PRIVATE gles2 glpp)

message(STATUS "copying shaders from ${PROJECT_SOURCE_DIR}/shaders to ${CMAKE_BINARY_DIR}/shaders")

add_custom_command(TARGET glgame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders)

add_custom_command(TARGET glgame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/thirdparty/angle/bin/$<CONFIGURATION>
        $<TARGET_FILE_DIR:glgame>)

# add_custom_command(TARGET glgame POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${PROJECT_SOURCE_DIR}/res
#         ${CMAKE_CURRENT_BINARY_DIR}/res)
