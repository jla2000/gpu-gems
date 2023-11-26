#version 460 core

layout(location = 0) in vec3 vertex_position;

uniform mat4 models[16];

void main() {
  gl_Position = models[gl_InstanceID] * vec4(vertex_position, 1.0);
}
