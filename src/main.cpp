#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "watcher.h"

GLuint program_id;

void LoadProgram() {
  program_id = CreateShaderProgram({
      CreateShader(GL_VERTEX_SHADER, "shaders/triangle.vert"),
      CreateShader(GL_FRAGMENT_SHADER, "shaders/triangle.frag"),
  });

  glUseProgram(program_id);
}

void OnResize(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void OnShaderFilesChanged() {
  auto old_program_id = program_id;

  try {
    std::cerr << "Reloading shaders" << std::endl;
    LoadProgram();
    glDeleteProgram(old_program_id);
  } catch (...) {
    std::cerr << "Failed to reload shaders" << std::endl;
  }
}

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
  glfwSetWindowSizeCallback(window, OnResize);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW." << std::endl;
    return 1;
  }

  // clang-format off
  GLfloat triangle_vertices[]{
      -0.5f, -0.5f, 0.0f,
      0.0f, 0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
  };
  // clang-format on

  GLuint vertexArray;
  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray);

  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  LoadProgram();
  Watcher shader_watcher{"shaders"};

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();

    if (shader_watcher.Poll())
      OnShaderFilesChanged();
  }

  glfwTerminate();

  return 0;
}
