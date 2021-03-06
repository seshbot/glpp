#
# GLAD
#

add_subdirectory(glad)


#
# Angle
#

if (WIN32)
    add_subdirectory(angle)
endif ()

#
# GLFW
#

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

if (WIN32)
    set(GLFW_USE_EGL ON CACHE BOOL "" FORCE)
    set(GLFW_CLIENT_LIBRARY glesv2 CACHE STRING "" FORCE)

    set(EGL_INCLUDE_DIR ${ANGLE_EGL_INCLUDE_DIR} CACHE INTERNAL "")
    set(EGL_LIBRARY ${ANGLE_EGL_LIBRARY} CACHE INTERNAL "")

    set(GLESv2_INCLUDE_DIR ${ANGLE_GLESv2_INCLUDE_DIR} CACHE INTERNAL "")
    set(GLESv2_LIBRARY ${ANGLE_GLESv2_LIBRARY} CACHE INTERNAL "")
endif ()

add_subdirectory(glfw-3.1.1)

#
# GLM
#

set(GLM_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/glm-0.9.5.4 CACHE INTERNAL "")

add_subdirectory(glm-0.9.5.4)

#
# SOIL2
#

if (WIN32)
    set(SOIL2_USE_GLES2 ON CACHE INTERNAL "")
    set(SOIL2_USE_ANGLE ON CACHE INTERNAL "")
    set(SOIL2_EGL_INCLUDE_DIR ${ANGLE_EGL_INCLUDE_DIR} CACHE INTERNAL "")
endif ()

add_subdirectory(soil2-20141119)
set(SOIL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/soil2-20141119/src CACHE INTERNAL "")

#
# AssImp
#

set(BUILD_SHARED_LIBS OFF)
set(LIB_INSTALL_DIR "lib")
set(ASSIMP_LIB_INSTALL_DIR "lib")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_STATIC_LIB ON CACHE INTERNAL "")
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_SAMPLES OFF CACHE INTERNAL "")

add_subdirectory(assimp-3.1.1)

set(ASSIMP_LIBRARIES assimp CACHE INTERNAL "")
set(ASSIMP_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/assimp-3.1.1/include CACHE INTERNAL "")
