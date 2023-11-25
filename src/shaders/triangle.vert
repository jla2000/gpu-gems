#version 460 core

layout(location = 0) in vec3 vertex_position;

out vec3 vertex_color;

const vec3 colors[] = vec3[](
  vec3(1.0f, 0.0f, 0.0f),
  vec3(0.0f, 1.0f, 0.0f),
  vec3(0.0f, 0.0f, 1.0f)
);

void main() {
  gl_Position = vec4(vertex_position, 1.0);
  vertex_color = colors[gl_VertexID];
}
