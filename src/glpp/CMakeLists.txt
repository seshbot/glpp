
add_definitions(-DGL_GLEXT_PROTOTYPES)

set(SOURCE
  utils.cpp   ${PROJECT_SOURCE_DIR}/include/glpp/utils.h
  input.cpp   ${PROJECT_SOURCE_DIR}/include/glpp/input.h
  glpp.cpp    ${PROJECT_SOURCE_DIR}/include/glpp/glpp.h
  scene.cpp   ${PROJECT_SOURCE_DIR}/include/glpp/scene.h
  assets.cpp  ${PROJECT_SOURCE_DIR}/include/glpp/assets.h
  detail/stb/stb_easy_font.h
  detail/ai_helpers.cpp detail/ai_helpers.h
  detail/ai_structures.cpp detail/ai_structures.h)

add_library(glpp ${SOURCE})

target_include_directories(glpp SYSTEM PUBLIC ${GLFW_SOURCE_DIR}/include ${GLFW_SOURCE_DIR}/deps)
target_include_directories(glpp SYSTEM PUBLIC ${GLM_INCLUDES})
target_include_directories(glpp SYSTEM PRIVATE ${SOIL_INCLUDES})
target_include_directories(glpp SYSTEM PRIVATE ${ASSIMP_INCLUDES})

target_link_libraries(glpp glfw soil ${OPENGL_GLU_LIBRARY} ${GLFW_LIBRARIES} ${ASSIMP_LIBRARIES})

if (WIN32)
   target_link_libraries(glpp gles2)

   include_directories(SYSTEM ${ANGLE_INCLUDES})
   target_link_libraries(glpp ${ANGLE_LIBRARIES})
else ()
   target_link_libraries(glpp gl2)
endif ()
