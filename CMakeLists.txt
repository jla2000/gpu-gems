cmake_minimum_required(VERSION 3.27)

project(gpu-gems)

#add_custom_command(
#  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/triangle_vert.h
#  COMMAND xxd -C -i -n triangle_vert ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/triangle.vert > ${CMAKE_CURRENT_BINARY_DIR}/triangle_vert.h
#  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/triangle.vert
#)
#add_custom_command(
#  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/triangle_frag.h
#  COMMAND xxd -C -i -n triangle_frag ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/triangle.frag > ${CMAKE_CURRENT_BINARY_DIR}/triangle_frag.h
#  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/triangle.frag
#)

set(CMAKE_CXX_STANDARD 20)

configure_file(src/shaders/triangle.vert ${CMAKE_CURRENT_BINARY_DIR}/triangle.vert)
configure_file(src/shaders/triangle.frag ${CMAKE_CURRENT_BINARY_DIR}/triangle.frag)

set(
  SOURCE_FILES 
  src/main.cpp
)

add_executable(gpu-gems ${SOURCE_FILES})
target_link_libraries(gpu-gems PRIVATE GL GLEW glfw)
