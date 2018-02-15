project(Particles)
cmake_minimum_required(VERSION 2.8)

find_package(PkgConfig REQUIRED)
pkg_search_module(ASSIMP REQUIRED assimp)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(GLEW REQUIRED)
find_package(OpenGL  REQUIRED)

include_directories(
  ${ASSIMP_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
  )

if(NOT MSVC)
  find_library(M_LIB m)
else() # not needed on Windows
  set(M_LIB "")
endif()


file(GLOB SOURCES
   include/*.h
   source/*.cpp
   textures/*.jpg
   shaders/*.frag
   shaders/*.vs
)

add_executable( ${PROJECT_NAME} ${SOURCES})
target_link_libraries(
  ${PROJECT_NAME}
  ${ASSIMP_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${M_LIB}
  libSOIL.a
  )
