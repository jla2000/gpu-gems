cmake_minimum_required(VERSION 3.27)

project(gpu-gems)

set(CMAKE_CXX_STANDARD 20)

configure_file(src/shaders/quad.vert ${CMAKE_CURRENT_BINARY_DIR}/shaders/quad.vert)
configure_file(src/shaders/quad.frag ${CMAKE_CURRENT_BINARY_DIR}/shaders/quad.frag)
configure_file(src/shaders/quad.geom ${CMAKE_CURRENT_BINARY_DIR}/shaders/quad.geom)

set(
  IMGUI_SOURCES
  $ENV{IMGUI_DIR}/imgui.cpp
  $ENV{IMGUI_DIR}/imgui_demo.cpp
  $ENV{IMGUI_DIR}/imgui_draw.cpp
  $ENV{IMGUI_DIR}/imgui_tables.cpp
  $ENV{IMGUI_DIR}/imgui_widgets.cpp
  $ENV{IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  $ENV{IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(
  SOURCE_FILES 
  src/main.cpp
  src/shader.cpp
  src/watcher.cpp
)

add_executable(gpu-gems ${SOURCE_FILES} ${IMGUI_SOURCES})
target_link_libraries(gpu-gems PRIVATE GL GLEW glfw)
