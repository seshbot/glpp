set(SOURCE
  commands.cpp
  extensions.cpp
  ${PROJECT_SOURCE_DIR}/include/glpp/gl2/commands.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gl2/extensions.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gl2/extensions_enums.h
  ${PROJECT_SOURCE_DIR}/include/glpp/gl2/enums.h
  ${PROJECT_SOURCE_DIR}/include/glpp/types.h)

add_library(gl2 ${SOURCE})

target_include_directories(gl2 PUBLIC ${PROJECT_SOURCE_DIR}/include)

