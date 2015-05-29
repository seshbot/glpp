
set(ASSET_DIR ${PROJECT_SOURCE_DIR}/res)

set(SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/db.h
  ${CMAKE_CURRENT_SOURCE_DIR}/game_db.h
  ${CMAKE_CURRENT_SOURCE_DIR}/game.cpp ${CMAKE_CURRENT_SOURCE_DIR}/game.h
  ${CMAKE_CURRENT_SOURCE_DIR}/particles_3d.h
  ${CMAKE_CURRENT_SOURCE_DIR}/render_3d.cpp ${CMAKE_CURRENT_SOURCE_DIR}/render_3d.h)

set(SHADERS
  ${ASSET_DIR}/shaders/2d.vert ${ASSET_DIR}/shaders/2d.frag
  ${ASSET_DIR}/shaders/3d.vert ${ASSET_DIR}/shaders/3d.frag
  ${ASSET_DIR}/shaders/3d_particle.vert ${ASSET_DIR}/shaders/3d_particle.frag
  ${ASSET_DIR}/shaders/3d_shadow.vert ${ASSET_DIR}/shaders/3d_shadow.frag
  ${ASSET_DIR}/shaders/post.vert ${ASSET_DIR}/shaders/post.frag
  ${ASSET_DIR}/shaders/sprite.vert ${ASSET_DIR}/shaders/sprite.frag)

add_executable(glgame WIN32 CMakeLists.txt ${SOURCE} ${SHADERS})

source_group(Shaders FILES ${SHADERS})

target_include_directories(glgame SYSTEM PRIVATE ${GLFW_SOURCE_DIR}/include ${GLFW_SOURCE_DIR}/deps)
target_include_directories(glgame SYSTEM PRIVATE ${ASSIMP_INCLUDES})

target_link_libraries(glgame PRIVATE glfw ${OPENGL_GLU_LIBRARY} ${GLFW_LIBRARIES} ${ASSIMP_LIBRARIES})
target_link_libraries(glgame PRIVATE glpp)

message(STATUS "copying shaders from ${ASSET_DIR}/shaders to ${CMAKE_BINARY_DIR}/shaders")

add_custom_command(TARGET glgame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSET_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders)


if (WIN32)
  add_custom_command(TARGET glgame POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${PROJECT_SOURCE_DIR}/thirdparty/angle/bin/$<CONFIGURATION>
          $<TARGET_FILE_DIR:glgame>)
endif ()

# add_custom_command(TARGET glgame POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${PROJECT_SOURCE_DIR}/res
#         ${CMAKE_CURRENT_BINARY_DIR}/res)
