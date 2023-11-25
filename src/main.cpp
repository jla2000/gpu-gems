#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "watcher.h"

GLuint program_id;
GLuint projection_id;
GLuint view_id;
GLuint model_id;

glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;

void UploadCameraMatrices() {
  glUniformMatrix4fv(projection_id, 1, GL_FALSE, &projection[0][0]);
  glUniformMatrix4fv(view_id, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(model_id, 1, GL_FALSE, &model[0][0]);
}

void SetupProjection(int width, int height) {
  projection = glm::perspective(glm::radians(45.0f),
                                (float)width / (float)height, 0.1f, 100.0f);
}

void SetupView(glm::vec3 position, glm::vec3 target) {
  view = glm::lookAt(position, target, glm::vec3{0, 1, 0});
}

void LoadProgram() {
  program_id = CreateShaderProgram({
      CreateShader(GL_VERTEX_SHADER, "shaders/triangle.vert"),
      CreateShader(GL_FRAGMENT_SHADER, "shaders/triangle.frag"),
  });

  projection_id = glGetUniformLocation(program_id, "projection");
  view_id = glGetUniformLocation(program_id, "view");
  model_id = glGetUniformLocation(program_id, "model");

  glUseProgram(program_id);
  UploadCameraMatrices();
}

void OnResize(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  SetupProjection(width, height);
  UploadCameraMatrices();
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

  SetupView({3, 3, 3}, {0, 0, 0});
  SetupProjection(800, 600);
  model = glm::mat4(1.0f);

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

  float lastFrame = glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();

    if (shader_watcher.Poll())
      OnShaderFilesChanged();

    float currentFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float amount = deltaTime * 100.0f;
    model = glm::rotate(model, glm::radians(amount), {0, 1, 0});
    UploadCameraMatrices();
  }

  glfwTerminate();

  return 0;
}
