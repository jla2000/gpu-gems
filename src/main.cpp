#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <poll.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>

#include "shader.h"

int main() {
  glfwSetErrorCallback([](int, char const *error_message) {
    std::cerr << "GLFW error: " << error_message << std::endl;
    std::exit(1);
  });

  if (!glfwInit()) {
    std::cerr << "Failed to intialize GLFW." << std::endl;
    return 1;
  }

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

  GLFWwindow *window = glfwCreateWindow(800, 600, "gpu-gems", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to open window." << std::endl;
    return 1;
  }
  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW." << std::endl;
    return 1;
  }

  GLuint vertexArray;
  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray);

  auto LoadProgram = []() {
    return CreateShaderProgram({
        CreateShader(GL_VERTEX_SHADER, "shaders/triangle.vert"),
        CreateShader(GL_FRAGMENT_SHADER, "shaders/triangle.frag"),
    });
  };

  GLuint program_id = LoadProgram();

  int watcher = inotify_init();
  if (watcher < 0) {
    std::cerr << "Failed to start watcher" << std::endl;
    return 1;
  }

  int watch = inotify_add_watch(watcher, "shaders", IN_MODIFY);
  if (watch < 0) {
    std::cerr << "Failed to watch shaders" << std::endl;
    return 1;
  }

  glClearColor(1.0, 0.0, 0.0, 1.0);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    glfwPollEvents();

    pollfd poll_fd{watcher, POLLIN, 0};
    if (poll(&poll_fd, 1, 0) > 0) {
      char event_buffer[(sizeof(inotify_event) + 16) * 16];
      int files_changed = read(watcher, event_buffer, sizeof(event_buffer));

      if (files_changed > 0) {
        std::cout << "Shader files changed. Reloading..." << std::endl;
        glDeleteProgram(program_id);
        program_id = LoadProgram();
      }
    }
  }

  inotify_rm_watch(watcher, watch);
  close(watcher);

  glfwTerminate();

  return 0;
}
