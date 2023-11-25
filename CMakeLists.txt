cmake_minimum_required(VERSION 3.27)

project(gpu-gems)

set(CMAKE_CXX_STANDARD 20)

configure_file(src/shaders/triangle.vert ${CMAKE_CURRENT_BINARY_DIR}/triangle.vert)
configure_file(src/shaders/triangle.frag ${CMAKE_CURRENT_BINARY_DIR}/triangle.frag)

set(
  SOURCE_FILES 
  src/main.cpp
  src/shader.cpp
)

add_executable(gpu-gems ${SOURCE_FILES})
target_link_libraries(gpu-gems PRIVATE GL GLEW glfw)
