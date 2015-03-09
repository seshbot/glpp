set(SOURCE
  gles2.cpp ${PROJECT_SOURCE_DIR}/include/glpp/gles2.h
  commands.cpp
  extensions.cpp
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/commands.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/extensions.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/extensions_enums.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gles2/enums.h
  ${PROJECT_SOURCE_DIR}/include/glpp/angle_ext.h
  ${PROJECT_SOURCE_DIR}/include/glpp/types.h)

if (WIN32)
  set(SOURCE ${SOURCE}
    angle_ext.cpp
    angle_extension_macros.h
  )
endif ()

add_library(gles2 ${SOURCE})

target_link_libraries(gles2 glad)
target_include_directories(gles2 PRIVATE ${GLAD_INCLUDE_DIR})

target_include_directories(gles2 PUBLIC ${PROJECT_SOURCE_DIR}/include glad/include)

if (WIN32)
    target_include_directories(gles2 SYSTEM PRIVATE ${ANGLE_INCLUDES})
    target_link_libraries(gles2 ${ANGLE_LIBRARIES})
endif ()
