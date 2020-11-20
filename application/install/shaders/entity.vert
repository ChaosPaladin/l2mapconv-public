#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in mat4 in_model;

out vec3 v_normals;
out vec2 v_uvs;
out vec3 v_positions;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    mat4 model = u_model * in_model;
    gl_Position = u_projection * u_view * model * vec4(in_position, 1.0f);

    v_normals = normalize(transpose(inverse(mat3(model))) * in_normal);
    v_uvs = in_uv;
    v_positions = vec3(model * vec4(in_position, 1.0f));
}
