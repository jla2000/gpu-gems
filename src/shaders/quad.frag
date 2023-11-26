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
  vec3 light_position = vec3(100, 100, 10);
  vec3 light_direction = normalize(light_position - position);

  vec3 normal = calculate_normal();

  vec3 ambient = vec3(0.1);
  vec3 diffuse = vec3(max(dot(normal, light_direction), 0));

  vec3 camera_pos = vec3(5, 5, 5);
  float specular_strength = 0.8;
  vec3 view_dir = normalize(camera_pos - position);
  vec3 reflect_dir = reflect(-light_direction, normal);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
  vec3 specular = specular_strength * spec * light_color;

  vec3 result = (ambient + diffuse + specular) * object_color;

  color = vec4(result, 1);
}
