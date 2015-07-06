
set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/texture_batches_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/triangle_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/buffer_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/text_demo.cpp)

add_executable(gltestbed WIN32 CMakeLists.txt ${SOURCE})

target_link_libraries(gltestbed PRIVATE glpp)

if (WIN32)
  add_custom_command(TARGET gltestbed POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${PROJECT_SOURCE_DIR}/thirdparty/angle/bin/$<CONFIGURATION>
          $<TARGET_FILE_DIR:gltestbed>)
endif ()
