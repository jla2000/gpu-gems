#version 330 core

out vec4 color;
in vec3 position;

uniform float time;

vec3 calculate_normal() {
  vec2 D = normalize(vec2(3, 1));

  float l = 0.4;
  float w = 2.0 / l;
  float a = 0.1;
  float phase = 0.7f * w;

  return normalize(vec3(
    -D.x * w * a * cos(dot(D, position.xz) * w + time * phase),
    1,
    -D.y * w * a * cos(dot(D, position.xz) * w + time * phase)
  ));
}

void main() {
  vec3 object_color = vec3(0, 0.5, 0.8);
  vec3 light_color = vec3(1, 1, 1);
  vec3 light_position = vec3(10, 100, 10);
  vec3 light_direction = normalize(light_position - position);

  vec3 normal = calculate_normal();

  float ambient = 0.1;
  float diffuse = max(dot(normal, light_direction), 0);

  vec3 result = (ambient + diffuse) * object_color;

  color = vec4(result, 1);
}
