#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>

#include "shader.h"
#include "watcher.h"

GLuint program_id;
GLuint view_projection_id;
GLuint models_id;
GLuint sub_divisions_id;
GLuint time_id;

glm::mat4 projection;
glm::mat4 view;
glm::mat4 models[16];

GLuint sub_divisions = 8;

float lastFrame;
float deltaTime;

void UpdateDeltaTime() {
  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
}

void UpdateProgramUniforms() {
  glm::mat4 view_projection = projection * view;
  glUniformMatrix4fv(view_projection_id, 1, GL_FALSE, &view_projection[0][0]);
  glUniformMatrix4fv(models_id, 16, GL_FALSE, &models[0][0][0]);
  glUniform1ui(sub_divisions_id, sub_divisions);
  glUniform1f(time_id, glfwGetTime());
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
      CreateShader(GL_VERTEX_SHADER, "shaders/quad.vert"),
      CreateShader(GL_FRAGMENT_SHADER, "shaders/quad.frag"),
      CreateShader(GL_GEOMETRY_SHADER, "shaders/quad.geom"),
  });

  models_id = glGetUniformLocation(program_id, "models");
  view_projection_id = glGetUniformLocation(program_id, "view_projection");
  sub_divisions_id = glGetUniformLocation(program_id, "sub_divisions");
  time_id = glGetUniformLocation(program_id, "time");

  glUseProgram(program_id);
  UpdateProgramUniforms();
}

void OnResize(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  SetupProjection(width, height);
  UpdateProgramUniforms();
}

void ReloadProgram() {
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

  SetupView({5, 5, 5}, {0, 0, 0});
  SetupProjection(800, 600);

  for (int x = 0; x < 4; ++x) {
    for (int z = 0; z < 4; ++z) {
      models[(x * 4) + z] = glm::translate(glm::mat4{1.0f}, {x - 2, 0, z - 2});
    }
  }

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW." << std::endl;
    return 1;
  }

  // clang-format off
  GLfloat quad_vertices[]{
    -0.5f, 0.0f, -0.5f,
    -0.5f, 0.0f, 0.5f,
    0.5f, 0.0f, 0.5f,
    0.5f, 0.0f, -0.5f
  };
  GLuint quad_indices[] {
    0, 1, 2,
    0, 2, 3,
  };
  // clang-format on

  GLuint vertexArray;
  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray);

  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  GLuint indexBuffer;
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices,
               GL_STATIC_DRAW);

  // Show lines
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Culling
  // glEnable(GL_CULL_FACE);
  // glFrontFace(GL_CW);
  // glCullFace(GL_FRONT);

  LoadProgram();
  Watcher shader_watcher{"shaders"};

  while (!glfwWindowShouldClose(window)) {
    UpdateDeltaTime();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElementsInstanced(GL_TRIANGLES,
                            sizeof(quad_indices) / sizeof(GLfloat),
                            GL_UNSIGNED_INT, nullptr, 16);

    glfwSwapBuffers(window);
    glfwPollEvents();

    if (shader_watcher.Poll())
      ReloadProgram();

    float amount = deltaTime * 100.0f;
    // models[0] = glm::rotate(models[0], glm::radians(amount), {0, 1, 0});
    UpdateProgramUniforms();
  }

  glfwTerminate();

  return 0;
}
