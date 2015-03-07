set(SOURCE
  gles2.cpp ${PROJECT_SOURCE_DIR}/include/glpp/gles2.h
  commands.cpp
  extensions.cpp
  angle_ext.cpp
  angle_extension_macros.h
  glad/src/glad.c glad/include/glad/glad.h glad/include/KHR/khrplatform.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/commands.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/extensions.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/extensions_enums.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/enums.h
  ${PROJECT_SOURCE_DIR}/include/glpp/angle_ext.h
  ${PROJECT_SOURCE_DIR}/include/glpp/types.h)

add_library(gles2 ${SOURCE})

target_include_directories(gles2 PUBLIC ${PROJECT_SOURCE_DIR}/include glad/include)

if (WIN32)
    target_include_directories(gles2 SYSTEM PRIVATE ${ANGLE_INCLUDES})
    target_link_libraries(gles2 ${ANGLE_LIBRARIES})
endif ()
