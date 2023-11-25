#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Parse a #include directive and return absolute file path
std::string ParseInclude(std::string const &filename, std::string const &line) {
  std::string basename = std::filesystem::path(filename).parent_path().string();
  std::string includedFile = line.substr(line.find('"') + 1, std::string::npos);
  return basename + '/' + includedFile.substr(0, includedFile.size() - 1);
}

// Load source code of a shader file
std::string LoadShader(std::string const &filename) {
  std::ifstream ifs{filename};

  if (!ifs.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  std::ostringstream oss;
  std::string line;

  while (std::getline(ifs, line)) {
    // Basic shader preprocessor
    if (line.starts_with("#include")) {
      line = LoadShader(ParseInclude(filename, line));
    }
    oss << line << "\n";
  }

  return oss.str();
}

void CheckCompileStatus(GLuint shader_id) {
  GLint status{GL_TRUE};
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);

  if (status == GL_FALSE) {
    GLint infoLogLength{0};
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> infoLog(infoLogLength + 1);
    glGetShaderInfoLog(shader_id, infoLogLength, nullptr, infoLog.data());
    std::cerr << infoLog.data() << std::endl;

    throw std::runtime_error("Failed to compile shader");
  }
}

void CheckLinkStatus(GLuint program_id) {
  GLint status{GL_TRUE};
  glGetProgramiv(program_id, GL_LINK_STATUS, &status);

  if (status == GL_FALSE) {
    GLint infoLogLength{0};
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> infoLog(infoLogLength + 1);
    glGetProgramInfoLog(program_id, infoLogLength, nullptr, infoLog.data());
    std::cerr << infoLog.data() << std::endl;

    throw std::runtime_error("Failed to link program");
  }
}

GLuint CreateShader(GLenum type, std::string const filename) {
  GLuint shader_id = glCreateShader(type);

  auto srcCode = LoadShader(filename);
  auto srcCodeCstr = srcCode.c_str();

  glShaderSource(shader_id, 1, &srcCodeCstr, nullptr);
  CheckCompileStatus(shader_id);

  return shader_id;
}

GLuint CreateShaderProgram(std::vector<GLuint> const shaders) {
  GLuint program_id = glCreateProgram();

  for (const auto shader_id : shaders) {
    glAttachShader(program_id, shader_id);
  }

  glLinkProgram(program_id);
  CheckLinkStatus(program_id);

  for (const auto shader_id : shaders) {
    glDetachShader(program_id, shader_id);
  }

  return program_id;
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

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW." << std::endl;
    return 1;
  }

  GLuint vertexArray;
  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray);

  glClearColor(1.0, 0.0, 0.0, 1.0);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
