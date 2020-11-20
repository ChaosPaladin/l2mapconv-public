#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 v_normals[];
in vec2 v_uvs[];
in vec3 v_positions[];

out vec3 v_normal;
out vec2 v_uv;
out vec3 v_position;

void main() {
  v_normal = normalize((v_normals[0] + v_normals[1] + v_normals[2]) / 3.0f);

  for (int i = 0; i < 3; ++i) {
    gl_Position = gl_in[i].gl_Position;
    v_uv = v_uvs[i];
    v_position = v_positions[i];
    EmitVertex();
  }

  EndPrimitive();
}
