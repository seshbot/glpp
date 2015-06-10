set(SOURCE
  gl2.cpp ${PROJECT_SOURCE_DIR}/include/glpp/gl2.h
  commands.cpp
  extensions.cpp
  osx/extensions.cpp
  ${PROJECT_SOURCE_DIR}/include/glpp/gl2/commands.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gl2/extensions.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gl2/extensions_enums.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gl2/enums.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gl2/osx/extensions.h
  ${PROJECT_SOURCE_DIR}/include/glpp/types.h)

add_library(gl2 ${SOURCE})

target_link_libraries(gl2 glad)
target_include_directories(gl2 PRIVATE ${GLAD_INCLUDE_DIR})

target_include_directories(gl2 PUBLIC ${PROJECT_SOURCE_DIR}/include)
