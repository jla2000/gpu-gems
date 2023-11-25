#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "watcher.h"

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
  Watcher shader_watcher{"shaders"};

  glClearColor(1.0, 0.0, 0.0, 1.0);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    glfwPollEvents();

    if (shader_watcher.FilesChanged()) {
      std::cout << "Shader files changed. Reloading..." << std::endl;
      glDeleteProgram(program_id);
      program_id = LoadProgram();
    }
  }

  glfwTerminate();

  return 0;
}
