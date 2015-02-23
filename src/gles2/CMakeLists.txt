add_definitions(-DGL_GLEXT_PROTOTYPES)

if (WIN32)
    add_definitions(-DGLPP_USING_ANGLE)
endif ()

set(SOURCE
  commands.cpp
  extensions.cpp
  angle_extension_macros.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/commands.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/extensions.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/extensions_enums.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/enums.h
  ${PROJECT_SOURCE_DIR}/include/glpp/types.h)

add_library(gles2 ${SOURCE})

target_include_directories(gles2 PUBLIC ${PROJECT_SOURCE_DIR}/include)

if (WIN32)
    target_include_directories(gles2 SYSTEM PRIVATE ${ANGLE_INCLUDES})
    target_link_libraries(gles2 ${ANGLE_LIBRARIES})
endif ()
