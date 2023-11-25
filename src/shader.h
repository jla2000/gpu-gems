#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>

GLuint CreateShader(GLenum type, std::string const filename);
GLuint CreateShaderProgram(std::vector<GLuint> const shaders);
