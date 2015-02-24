
add_definitions(-DGL_GLEXT_PROTOTYPES)

set(SOURCE
  utils.cpp   ${PROJECT_SOURCE_DIR}/include/glpp/utils.h
  input.cpp   ${PROJECT_SOURCE_DIR}/include/glpp/input.h
  glpp.cpp    ${PROJECT_SOURCE_DIR}/include/glpp/glpp.h)

add_library(glpp ${SOURCE})

target_include_directories(glpp SYSTEM PUBLIC ${GLFW_SOURCE_DIR}/include ${GLFW_SOURCE_DIR}/deps)
target_include_directories(glpp SYSTEM PUBLIC ${GLM_INCLUDES})
target_include_directories(glpp SYSTEM PRIVATE ${SOIL_INCLUDES})

target_link_libraries(glpp glfw soil ${OPENGL_GLU_LIBRARY} ${GLFW_LIBRARIES})
target_link_libraries(glpp gles2)

if (WIN32)
    include_directories(SYSTEM ${ANGLE_INCLUDES})
    
    add_definitions(-DGLPP_USING_ANGLE)

    target_link_libraries(glpp ${ANGLE_LIBRARIES})
else ()
    include_directories(SYSTEM ${GLEW_INCLUDES})

    target_link_libraries(glpp ${GLEW_LIBRARIES})
endif ()